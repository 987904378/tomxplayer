//
//  main.c
//
//  Author:
//       Jonathan Jason Dennis <theonejohnnyd@gmail.com>
//
//  Copyright (c) 2017 Meticulus Development
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <gtk/gtk.h>
#include "op_control.h"
#include "settings.h"
#include "log.h"
#include "main_settings.h"
#include "preference_dialog.h"
#include "list.h"
#include "media_playlist.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include "time_utils.h"
#include <libgen.h>
#include <sys/types.h>
#include <signal.h>
#include "url_dialog.h"
#include "ytdl_control.h"
#include "top_widget.h"

#ifdef GTK3
#include "gtk3_compat.h"
#endif

#define TAG "main"

#ifndef RESOURCE_DIR
#define RESOURCE_DIR "."
#endif
#define ICON_PATH RESOURCE_DIR"/tomxplayer.png"

#ifdef GTK3
GMainContext *context;
#endif

static GtkWidget * window;
static top_widget_t *topw;
static GtkWidget *top_controls;
static GtkWidget *top_toolbar;
static GtkToolItem *fullscreen;
static gboolean _fullscreen = FALSE;
static gboolean _focused = TRUE;
static gboolean _dialog_showing = FALSE;

static pthread_t fs_hide_controls_thread;
#ifdef GTK3
static GtkCssProvider *fs_css_provider;
static const char *fs_css = "GtkWindow, GtkToolbar, GtkLabel, GtkDrawingArea { background: #000000; color: #FFFFFF; }";
#else
static GtkStyle *form_orig;
static GtkStyle *label_orig;
static GtkStyle *toolbar_orig;
static GtkStyle *form_fs;
static GtkStyle *label_fs;
static GtkStyle *toolbar_fs;
#endif
static list_t audio_settings;
static list_t playback_settings;
#ifndef NO_OSD
static list_t osd_settings;
#endif
static list_t win_settings;
static list_t advanced_settings;

static void show_controls() {
	gtk_widget_show(top_controls);
	top_widget_showcontrols(topw);
}
#ifdef GTK3
static gboolean hide_controls(gpointer not_used) {
#else
static void hide_controls() {
#endif
	gtk_widget_hide(top_controls);
	top_widget_hidecontrols(topw);
#if GTK3
	return FALSE;
#endif
}

static void destroy( GtkWidget *widget, gpointer data ) {
	top_widget_destroy(topw);
	gtk_main_quit ();
}

gboolean window_focus_out_event (GtkWidget *widget, GdkEventFocus *event, gpointer user_data) {
	_focused = FALSE;
	if(win_trans_unfocus.int_value && !topw->op_widget->minimized && !topw->hidden) {
		top_widget_set_alpha(topw, win_trans_alpha.int_value);
#ifdef GTK3
		gtk_widget_set_opacity ((GtkWidget *)window, (double)win_trans_alpha.int_value / 255);
#endif
	}
	return FALSE;
}

gboolean window_focus_in_event (GtkWidget *widget, GdkEventFocus *event, gpointer user_data) {
	_focused = TRUE;
	if(win_trans_unfocus.int_value && !topw->hidden) {
		top_widget_set_alpha(topw, 255);
#ifdef GTK3
		gtk_widget_set_opacity((GtkWidget *)window, 1);
#endif
	}
	return FALSE;
}

static void * timed_hide_controls(void * arg) {
    int c = 0;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	while(c < 499) {
		usleep(10000);
		c++;
	}
	if(_fullscreen && !_dialog_showing) {
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
#ifdef GTK3
		g_main_context_invoke(context, &hide_controls, NULL);
#else
		gdk_threads_enter();
		hide_controls();
		gdk_threads_leave();
#endif
	}
	return NULL;
}

static gboolean window_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer user_data) {
	show_controls(); 
	if(_fullscreen && !_dialog_showing) {
		pthread_cancel(fs_hide_controls_thread);
		pthread_join(fs_hide_controls_thread, NULL);
		pthread_create(&fs_hide_controls_thread,NULL, &timed_hide_controls,NULL);
	}
	return FALSE;
}

void *play_on_start(void *arg) {
	while(!top_widget_is_ready(topw))
		usleep(500 * 1000);
	top_widget_play_path(topw);
	return NULL;
}

static void window_realize(GtkWidget *widget, gpointer data) {
	if(topw->playlist != NULL && topw->playlist->list.count > 0) {
		pthread_t thread;
		pthread_create(&thread, NULL, &play_on_start, NULL);
	}
}

static void preferences_clicked( GtkWidget *widget, gpointer data ) {
	top_widget_hidevideo(topw);
	_dialog_showing = TRUE;
	preference_dialog_t *pd = gtk_preference_dialog_new((GtkWindow *)window);
	gtk_preference_dialog_add(pd, &audio_settings);
	gtk_preference_dialog_add(pd, &playback_settings);
#ifndef NO_OSD
	gtk_preference_dialog_add(pd, &osd_settings);
#endif
	gtk_preference_dialog_add(pd, &win_settings);
	gtk_preference_dialog_add(pd, &advanced_settings);
	gtk_dialog_run((GtkDialog *) pd->window);
	gtk_widget_destroy((GtkWidget *)pd->window);
	gtk_preference_dialog_free(pd);
	top_widget_unhidevideo(topw);
	_dialog_showing = FALSE;
}

static void ytdl_out_cb(char *out_line) {
#ifndef NO_OSD
	top_widget_osd_show(topw, out_line);
#endif
}

static void ytdl_url_cb(char *url) {
	top_widget_set_video_path(topw, url);
	top_widget_play_path(topw);
}

static void url_clicked( GtkWidget *widget, gpointer data ) {
	top_widget_hidevideo(topw);
	_dialog_showing = TRUE;
	url_dialog_t *ud = gtk_url_dialog_new((GtkWindow *)window);
	int response = gtk_dialog_run (GTK_DIALOG (ud->window));
	gtk_widget_destroy((GtkWidget *)ud->window);
	if (response == GTK_RESPONSE_ACCEPT) {
		if(ud->url != NULL) {
			top_widget_set_video_path(topw, ud->url);
			top_widget_play_path(topw);
		}
	} else if (response == GTK_RESPONSE_APPLY) {
		if(ud->url != NULL) {
			_dialog_showing = FALSE;
			ytdl_register_output_cb(&ytdl_out_cb);
			ytdl_register_url_cb(&ytdl_url_cb);
			ytdl_cget_url_thread(ud->url);
			top_widget_stop(topw);
		}
	}
	top_widget_unhidevideo(topw);
	_dialog_showing = FALSE;
	window_motion_notify_event(NULL, NULL, NULL);
}

static void fullscreen_clicked( GtkWidget *widget, gpointer data ) {
	if(!_fullscreen) {
		gtk_window_fullscreen((GtkWindow *)window);
#ifndef GTK3
		if(form_orig != NULL) {
			g_object_unref(form_orig);
			g_object_unref(form_fs);
			g_object_unref(label_orig);
			g_object_unref(label_fs);
			g_object_unref(toolbar_orig);
			g_object_unref(toolbar_fs);
		}

		form_orig = gtk_style_copy(gtk_widget_get_style(window));
		label_orig = gtk_style_copy(gtk_widget_get_style(topw->time_label));
		toolbar_orig = gtk_style_copy(gtk_widget_get_style(top_toolbar));

		form_fs = gtk_style_copy(form_orig);
		form_fs->bg[0] = form_fs->black;
		label_fs = gtk_style_copy(label_orig);
		label_fs->fg[0] = label_fs->white;
		toolbar_fs = gtk_style_copy(toolbar_orig);
		toolbar_fs->bg[0] = toolbar_fs->black;

		gtk_widget_set_style(window, form_fs);
		gtk_widget_set_style(top_toolbar, toolbar_fs);
		gtk_widget_set_style(topw->pb_controls, toolbar_fs);
		gtk_widget_set_style(topw->vol_controls, toolbar_fs);
		gtk_widget_set_style(topw->time_label, label_fs);
		gtk_widget_set_style(topw->volume_label, label_fs);
		hide_controls();
#else
		gtk_style_context_add_provider
				(gtk_widget_get_style_context((GtkWidget *)window),(GtkStyleProvider *)fs_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
		gtk_style_context_add_provider
				(gtk_widget_get_style_context((GtkWidget *)top_toolbar),(GtkStyleProvider *)fs_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
		gtk_style_context_add_provider
				(gtk_widget_get_style_context((GtkWidget *)topw->pb_controls),(GtkStyleProvider *)fs_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
		gtk_style_context_add_provider
				(gtk_widget_get_style_context((GtkWidget *)topw->vol_controls),(GtkStyleProvider *)fs_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
		hide_controls(NULL);
#endif
		_fullscreen = TRUE;
	} else {
		_fullscreen = FALSE;
		gtk_window_unfullscreen((GtkWindow *)window);
#ifdef GTK3
		gtk_style_context_remove_provider(gtk_widget_get_style_context((GtkWidget *)window),(GtkStyleProvider *)fs_css_provider);
		gtk_style_context_remove_provider(gtk_widget_get_style_context((GtkWidget *)top_toolbar),(GtkStyleProvider *)fs_css_provider);
		gtk_style_context_remove_provider(gtk_widget_get_style_context((GtkWidget *)topw->pb_controls),(GtkStyleProvider *)fs_css_provider);
		gtk_style_context_remove_provider(gtk_widget_get_style_context((GtkWidget *)topw->vol_controls),(GtkStyleProvider *)fs_css_provider);
#else
		gtk_widget_set_style(window, form_orig);
		gtk_widget_set_style(top_toolbar, toolbar_orig);
		gtk_widget_set_style(topw->pb_controls, toolbar_orig);
		gtk_widget_set_style(topw->vol_controls, toolbar_orig);
		gtk_widget_set_style(topw->time_label, label_orig);
		gtk_widget_set_style(topw->volume_label, label_orig);
#endif
		show_controls();
	}
}

static gboolean window_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	/* Block so that keys do not effect controls. */
	return TRUE;
}

static gboolean window_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	LOGD(TAG,"Key release '%d'",event->keyval);
	switch(event->keyval) {
		case 112:
			top_widget_toggle_playpause(topw);
			break;
		case 65362:
			top_widget_volume_up(topw);
			break;
		case 65364:
			top_widget_volume_down(topw);
			break;
		case 65361:
			top_widget_seek_back(topw);
			break;
		case 65363:
			top_widget_seek_forward(topw);
			break;
		case 65366:
			top_widget_next(topw);
			break;
		case 65365:
			top_widget_previous(topw);
			break;
		case 65307:
			if(_fullscreen)
				gtk_toggle_tool_button_set_active
					((GtkToggleToolButton *)fullscreen, FALSE);
			break;
		case 65480:
			gtk_toggle_tool_button_set_active
				((GtkToggleToolButton *)fullscreen, _fullscreen ? FALSE : TRUE);
			break;
	}
	return TRUE;
}

gboolean video_filter_cb(const GtkFileFilterInfo *filter_info, gpointer data) {
	return (gboolean)is_media_by_ext(filter_info->filename) ? TRUE : FALSE;
}

static void file_open_clicked( GtkWidget *widget, gpointer data ) {
	GtkWidget *dialog;
	_dialog_showing = TRUE;
	top_widget_hidevideo(topw);
	dialog = gtk_file_chooser_dialog_new (
			"Open File", 
			(GtkWindow *)window,
			GTK_FILE_CHOOSER_ACTION_OPEN,
#ifdef GTK3
			"_Cancel",
#else
			GTK_STOCK_CANCEL,
#endif
			GTK_RESPONSE_CANCEL,
#ifdef GTK3
			"_Open",
#else
			GTK_STOCK_OPEN,
#endif 
			GTK_RESPONSE_ACCEPT,
			NULL);
	GtkFileFilter *video_filter = gtk_file_filter_new ();
	gtk_file_filter_add_custom (video_filter,
                        GTK_FILE_FILTER_FILENAME,
                        &video_filter_cb,
                        NULL,
                        NULL);
    gtk_file_filter_set_name (video_filter, "Video Files");
    gtk_file_chooser_add_filter ((GtkFileChooser *)dialog, video_filter);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gtk_widget_destroy (dialog);
		top_widget_unhidevideo(topw);
		top_widget_set_video_path(topw, filename);
		top_widget_play_path(topw);
		g_free (filename);
	} else {
		gtk_widget_destroy (dialog);
		top_widget_unhidevideo(topw);
	}
	_dialog_showing = FALSE;
	window_motion_notify_event(NULL, NULL, NULL);
}

static void about_clicked(GtkWidget *widget, gpointer user_data) {
#ifdef GTK3
	const char *authors[] = { "<a href=\"mailto:theonejohnnyd@gmail.com\">Jonathan Dennis</a>", NULL };
#else
	const char *authors[] = { "Jonathan Dennis <theonejohnnyd@gmail.com>", NULL };

#endif
	_dialog_showing = TRUE;
	GtkAboutDialog *ad = (GtkAboutDialog *)gtk_about_dialog_new();
	gtk_window_set_transient_for((GtkWindow *)ad, (GtkWindow *)window);
	gtk_about_dialog_set_program_name(ad, "Tactical OMXPlayer Gtk (tomxplayer)");
	gtk_about_dialog_set_version(ad, VERSION);
	gtk_about_dialog_set_copyright(ad, "Copyright (c) 2017 Meticulus Development\n"
						"Copyright (c) 2012, Broadcom Europe Ltd");
	gtk_about_dialog_set_comments(ad,"GUI video player for the Raspberry Pi / wrapper for omxplayer");
#ifdef GTK3
	gtk_about_dialog_set_license_type(ad, GTK_LICENSE_GPL_3_0);
#endif

	gtk_about_dialog_set_website(ad, "http://meticulus.co.vu");
	gtk_about_dialog_set_authors(ad, authors);
	top_widget_hidevideo(topw);
	gtk_dialog_run((GtkDialog *) ad);
	gtk_widget_destroy((GtkWidget *)ad);
	top_widget_unhidevideo(topw);
	_dialog_showing = FALSE;
	window_motion_notify_event(NULL, NULL, NULL);
}

static void build_drawing_area(GtkBox *vbox) {
#ifdef GTK3
	//TODO: MOVE ME!
	GError *gerr = NULL;
	fs_css_provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data (fs_css_provider, fs_css, strlen(fs_css), &gerr);
#endif
}

static void build_top_toolbar(GtkBox *vbox) {
	top_controls = gtk_hbox_new(0, 6);
	top_toolbar = gtk_toolbar_new();
	gtk_toolbar_set_show_arrow((GtkToolbar *)top_toolbar,FALSE);

	GtkToolItem *file_open = gtk_tool_button_new_from_stock("gtk-open");
	g_signal_connect((GObject *)file_open,"clicked",G_CALLBACK(file_open_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)top_toolbar,file_open, 0);
#ifdef GTK3
	GtkToolItem *url_open = gtk_tool_button_new_from_stock("applications-internet");
#else
	GtkToolItem *url_open = gtk_tool_button_new_from_stock("gtk-save-as");
#endif
	g_signal_connect((GObject *)url_open,"clicked",G_CALLBACK(url_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)top_toolbar,url_open, 1);

	fullscreen = gtk_toggle_tool_button_new_from_stock("gtk-fullscreen");
	g_signal_connect((GObject *)fullscreen,"clicked",G_CALLBACK(fullscreen_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)top_toolbar,fullscreen, 2);

	GtkToolItem *preferences = gtk_tool_button_new_from_stock("gtk-properties");
	g_signal_connect((GObject *)preferences,"clicked",G_CALLBACK(preferences_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)top_toolbar, preferences, 3);

	GtkToolItem *about = gtk_tool_button_new_from_stock("gtk-about");
	g_signal_connect((GObject *)about,"clicked",G_CALLBACK(about_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)top_toolbar, about, 4);

	gtk_box_pack_start((GtkBox *)top_controls, top_toolbar, FALSE, FALSE, 0);
	gtk_box_pack_start((GtkBox *)vbox, top_controls, FALSE, FALSE, 0);
}

static void build_bottom_toolbar(GtkBox *vbox) {
	topw = top_widget_new((GtkWindow *)window);
	gtk_box_pack_start(vbox, topw->widget, TRUE, TRUE, 0);
}

static void stretch_updated(void *setting, void *user_data) {
	setting_t *set = (setting_t *)setting;
	op_widget_set_aspect(set->int_value ? "stretch":"Letterbox");
	LOGD(TAG, "%s", "stretch updated");
}
#ifndef NO_OSD
static void osd_textsize_updated(void *setting, void *user_data) {
	if(osd_textsize_percent.int_value) {
		double percent = osd_textsize.int_value * 0.01;
		top_widget_osd_set_text_size_percent(topw, percent);
	} else
		top_widget_osd_set_text_size(topw, (unsigned int)osd_textsize.int_value);
	LOGD(TAG, "%s", "osd_text_size updated");
}
#endif

static void win_trans_setting_updated(void *setting, void *user_data) {
	if(win_trans_unfocus.int_value && !_focused && !topw->hidden) {
		top_widget_set_alpha(topw, win_trans_alpha.int_value);
#ifdef GTK3
		if(window)
			gtk_widget_set_opacity (window, (double)win_trans_alpha.int_value / 255);
#endif
	} else {
		top_widget_set_alpha(topw, 255);
		top_widget_hidevideo(topw);
#ifdef GTK3
		if(window)
			gtk_widget_set_opacity ((GtkWidget *)window, 1);
#endif
	}
}

#ifdef USE_SIGHANDLER
static void signal_handler(int sig) {
	LOGE(TAG, "Recieved signal %d stopping omxplayer",sig);
	top_widget_destroy(topw);
	exit(127);
}
#endif

static void init_settings() {
	settings_init();
	settings_read(&cont_pb);
	settings_read(&volume);
	stretch.setting_update_cb = &stretch_updated;
	settings_read(&stretch);
	settings_read(&audio_out);
	settings_read(&file_types);
#ifndef NO_OSD
	settings_read(&osd_enable);
	osd_textsize.setting_update_cb = &osd_textsize_updated;
	settings_read(&osd_textsize);
	osd_textsize_percent.setting_update_cb = &osd_textsize_updated;
	settings_read(&osd_textsize_percent);
#endif
	win_trans_unfocus.setting_update_cb = & win_trans_setting_updated;
	settings_read(&win_trans_unfocus);
	win_trans_alpha.setting_update_cb = & win_trans_setting_updated;
	settings_read(&win_trans_alpha);
	settings_read(&omx_extra_args);
	settings_read(&arb_x_offset);
	settings_read(&arb_y_offset);

	audio_settings = list_create("Audio",2,1);
	list_add_entry(&audio_settings,&volume);
	list_add_entry(&audio_settings, &audio_out);
	playback_settings = list_create("Playback",3,1);
	list_add_entry(&playback_settings, &cont_pb);
	list_add_entry(&playback_settings, &stretch);
	list_add_entry(&playback_settings, &file_types);
#ifndef NO_OSD
	osd_settings = list_create("On-Screen Display", 2,1);
	list_add_entry(&osd_settings, &osd_enable);
	list_add_entry(&osd_settings, &osd_textsize);
	list_add_entry(&osd_settings, &osd_textsize_percent);
#endif
	win_settings = list_create("Window",2,1);
	list_add_entry(&win_settings, &win_trans_unfocus);
	list_add_entry(&win_settings, &win_trans_alpha);
	advanced_settings = list_create("Advanced",3,1);
	list_add_entry(&advanced_settings, &omx_extra_args);
#ifdef POLLWINPOS
	settings_read(&border_offset);
	settings_read(&title_bar_offset);
	list_add_entry(&advanced_settings, &border_offset);
	list_add_entry(&advanced_settings, &title_bar_offset);
#endif
	list_add_entry(&advanced_settings, &arb_x_offset);
	list_add_entry(&advanced_settings, &arb_y_offset);

}

int main (int argc, char * argv[]) {
#ifdef USE_SIGHANDLER
	signal(SIGSEGV, &signal_handler);
	signal(SIGQUIT, &signal_handler);
	signal(SIGILL, &signal_handler);
	signal(SIGABRT, &signal_handler);
#endif
	int opt;
	while ((opt = getopt(argc, argv, "v")) != -1) {
		switch (opt) {
			case 'v':
				printf("%s\n",VERSION);
				return 0;
				break;
		}
	}
	GError *gerr = NULL;
	gtk_init(0 ,NULL);
#ifdef GTK3
	context = g_main_context_default();
#else
	gdk_threads_init();
#endif
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow *)window,"tomxplayer");
	GdkPixbuf *icon = gdk_pixbuf_new_from_file(ICON_PATH, &gerr);
	gtk_window_set_icon((GtkWindow *)window,icon);
	gtk_window_set_default_icon(icon);
	gtk_window_stick((GtkWindow *)window);
	gtk_window_set_keep_above((GtkWindow *)window,TRUE);
	gtk_widget_set_events((GtkWidget *)window, GDK_ALL_EVENTS_MASK);
	g_signal_connect((GObject *)window, "destroy",G_CALLBACK(destroy), NULL);
	g_signal_connect((GObject *)window, "motion-notify-event", G_CALLBACK(window_motion_notify_event),NULL);
	g_signal_connect((GObject *)window, "key-press-event", G_CALLBACK(window_key_press_event),NULL);
	g_signal_connect((GObject *)window, "key-release-event", G_CALLBACK(window_key_release_event),NULL);
	g_signal_connect((GObject *)window, "focus-in-event", G_CALLBACK(window_focus_in_event),NULL);
	g_signal_connect((GObject *)window, "focus-out-event", G_CALLBACK(window_focus_out_event),NULL);
	g_signal_connect((GObject *)window, "realize", G_CALLBACK(window_realize),NULL);
	gtk_window_set_default_size((GtkWindow *)window, 640, 360);
	GtkWidget *vbox = gtk_vbox_new(0, 0); 
	gtk_container_add((GtkContainer *)window,vbox);
	build_top_toolbar((GtkBox *)vbox);
	build_drawing_area((GtkBox *)vbox); 
	build_bottom_toolbar((GtkBox *)vbox);
	if(argc > 1)
		top_widget_set_video_path(topw, argv[argc -1]);
	gtk_widget_show_all(window);
	init_settings();
	gtk_main();
	return 0;
}
