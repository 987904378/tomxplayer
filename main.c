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
#include "version.h"
#include "time_utils.h"

#define TAG "main"

static GtkWidget * window;
static GtkWidget *drawing_area;
static GtkWidget *hscale;
static GtkWidget *time_label;
static GtkWidget *volume_label;
static GtkWidget *bottom_controls;
static GtkWidget *pb_controls;
static GtkWidget *vol_controls;
static GtkWidget *top_controls;
static GtkWidget *top_toolbar;
static int window_x = 0;
static int window_y = 0;
static int da_x = 0;
static int da_y = 0;
static int da_w = 0;
static int da_h = 0;

static int pos[4];
static gboolean _fullscreen = FALSE;

static void play_path();

static pthread_t pb_pos_poll_thread;
static int pb_pos_poll_cancel = 0;
static int pb_pos_poll_running = 0;

static pthread_t fs_hide_controls_thread;
static pthread_t restore_volume_thread;

static GtkStyle *form_orig;
static GtkStyle *label_orig;
static GtkStyle *toolbar_orig;
static GtkStyle *form_fs;
static GtkStyle *label_fs;
static GtkStyle *toolbar_fs;

static list_t audio_settings;
static list_t playback_settings;

static media_playlist_t *_playlist;

static void show_controls() {
	gtk_widget_show(top_controls);
	gtk_widget_show(bottom_controls);
}
static void hide_controls() {
	gtk_widget_hide(top_controls);
	gtk_widget_hide(bottom_controls);
}

static void destroy( GtkWidget *widget, gpointer data ) {
	pb_pos_poll_cancel = 1;
	pthread_cancel(pb_pos_poll_thread);
	pthread_join(pb_pos_poll_thread, NULL);
	opc_stop_omxplayer();
	gtk_main_quit ();
}

static gboolean event_window_state (GtkWidget *widget, GdkEventWindowState *event, gpointer user_data) {
	if(event->new_window_state & GDK_WINDOW_STATE_ICONIFIED) {
		opc_hidevideo();
	} else { 
		opc_unhidevideo();
	}
	return FALSE;
}

static void * timed_hide_controls(void * arg) {
    int c = 0;
	pthread_detach(pthread_self());
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	while(c < 499) {
		usleep(10000);
		c++;
	}
	if(_fullscreen) {
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
		gdk_threads_enter();
		hide_controls();
		gdk_threads_leave();
	}
}

static gboolean window_motion_notify_event(GtkWidget *widget, GdkEventMotion *event, gpointer user_data) {
	show_controls(); 
	if(_fullscreen) {
		pthread_cancel(fs_hide_controls_thread);
		pthread_create(&fs_hide_controls_thread,NULL, &timed_hide_controls,NULL);
	}
	return FALSE;
}

static gboolean hscale_change_value(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data) {
	opc_set_pb_position((unsigned long)value);
	return FALSE;
}

static gboolean ff_clicked( GtkWidget *widget, gpointer data ) {
	unsigned long newpos = (unsigned long) gtk_range_get_value((GtkRange *)hscale);
	newpos += 10 * 1000 * 1000;
	opc_set_pb_position(newpos);
	return FALSE;
}

static int do_volume(int vol) {
	int ret = 0;
	double dvol = (double)vol / 100;
	ret = opc_set_volume(dvol);
	if(!ret)
		settings_save(&volume);
	return ret;
}

static gboolean vol_up_clicked( GtkWidget *widget, gpointer data ) {
	volume.int_value += 5;
	volume.int_value = volume.int_value > volume.max ? volume.max : volume.int_value;
	do_volume(volume.int_value);
	return FALSE;
}

static gboolean vol_down_clicked( GtkWidget *widget, gpointer data ) {
	volume.int_value -= 5;
	volume.int_value = volume.int_value < volume.min ? volume.min : volume.int_value;
	do_volume(volume.int_value);
	return FALSE;
}

static gboolean rewind_clicked( GtkWidget *widget, gpointer data ) {
	unsigned long newpos = (unsigned long) gtk_range_get_value((GtkRange *)hscale);
	newpos -= 10 * 1000 * 1000;
	opc_set_pb_position(newpos);
	return FALSE;
}

static void calc_render_pos() {
	pos[0] = window_x + da_x;
	pos[1] = window_y + da_y;
	pos[2] = pos[0] + da_w;
	pos[3] = pos[1] + da_h;
	opc_update_pos(pos); 
}

static void pause_clicked( GtkWidget *widget, gpointer data ) {
	opc_toggle_playpause();
}

static void * restore_volume(void * arg) {
	pthread_detach(pthread_self());
	while(do_volume(volume.int_value))
		usleep(100 * 1000);
}

static void *pb_pos_poll(void * arg) {
	char dur[255];
	char pos[255];
	long long pb_pos[2];
	char timestamp[255];
	pthread_detach(pthread_self());
	pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pb_pos_poll_running = 1; 
	while(!pb_pos_poll_cancel) {
		usleep(800 * 1000);
		if(!opc_is_running() && cont_pb.int_value && !pb_pos_poll_cancel) {
			mp_move_next(_playlist);
			play_path();
		}
		if(opc_status(pb_pos))
			continue;
		if(pb_pos_poll_cancel)
			break;
		gdk_threads_enter();
		gtk_range_set_range((GtkRange *)hscale, 0, (gdouble)pb_pos[0]);
		gtk_range_set_value((GtkRange *)hscale, (gdouble)pb_pos[1]);
		if(!ms_to_time(pb_pos[0],dur) && !ms_to_time(pb_pos[1],pos)) {
			sprintf(timestamp,"%s / %s",pos,dur);
			gtk_label_set_text((GtkLabel *)time_label,timestamp);
		}
		gdk_threads_leave();
	}
	pb_pos_poll_running = 0;
}

static int set_video_path(char *path) {
	FILE * fd = fopen(path,"r");
	if(fd == NULL) {
		int err = errno;
		LOGE(TAG, "path '%s' - %s", path, strerror(err));
		return err;
	}
	LOGD(TAG, "Video path set to %s",path);
	if(_playlist != NULL)
		mp_free(_playlist);
	if(cont_pb.int_value)
		_playlist = mp_create_dir_of_file(path);
	else {
		_playlist = mp_create();
		mp_add(_playlist, path);
	}
	return 0;
}

static void play_path() {
	char title[255];
	char vpath[255];
	if(!_playlist) {
		LOGE(TAG,"%s", "_playlist is NULL?");
		return;
	}
	mp_get_current(_playlist, vpath);
	sprintf(title, "%s - %s","tomxplayer", vpath);
	gtk_window_set_title((GtkWindow *)window, title);
	opc_start_omxplayer_thread(pos, vpath);
	if(!pb_pos_poll_running) {
		pthread_create(&pb_pos_poll_thread,NULL, &pb_pos_poll,NULL); 
	}
	sleep(1);
	pthread_create(&restore_volume_thread,NULL, &restore_volume,NULL);
}



static void previous_clicked( GtkWidget *widget, gpointer data ) {
	if(cont_pb.int_value) {
		unsigned long pos = (unsigned long) gtk_range_get_value((GtkRange *)hscale);
		if(pos > (1000 * 5000))
			opc_set_pb_position(0);
		else {
			mp_move_previous(_playlist);
			play_path();
		}
	} else {
		opc_set_pb_position(0);
	}
}

static void next_clicked( GtkWidget *widget, gpointer data ) {
	if(cont_pb.int_value) {
		mp_move_next(_playlist);
		play_path();
	}
}

static void preferences_clicked( GtkWidget *widget, gpointer data ) {
	opc_hidevideo();
	preference_dialog_t *pd = gtk_preference_dialog_new((GtkWindow *)window);
	gtk_preference_dialog_add(pd, &audio_settings);
	gtk_preference_dialog_add(pd, &playback_settings);
	gtk_dialog_run((GtkDialog *) pd->window);
	gtk_widget_destroy((GtkWidget *)pd->window);
	gtk_preference_dialog_free(pd);
	opc_unhidevideo();
}

static void fullscreen_clicked( GtkWidget *widget, gpointer data ) {
	if(!_fullscreen) {
		gtk_window_fullscreen((GtkWindow *)window);
		GdkColor black;
		black.red = 0;
		black.green = 0;
		black.blue = 0;
		GdkColor white;
		white.red = 65535;
		white.green = 65535;
		white.blue = 65535;
		form_orig =  gtk_widget_get_style(window);
		label_orig = gtk_widget_get_style(time_label);
		toolbar_orig = gtk_widget_get_style(top_toolbar);
		if(form_fs == NULL) {
			gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &black);
			gtk_widget_modify_bg(top_toolbar, GTK_STATE_NORMAL, &black);
			gtk_widget_modify_bg(pb_controls, GTK_STATE_NORMAL, &black);
			gtk_widget_modify_bg(vol_controls, GTK_STATE_NORMAL, &black);
			gtk_widget_modify_fg(time_label, GTK_STATE_NORMAL, &white);
			gtk_widget_modify_fg(volume_label, GTK_STATE_NORMAL, &white);
		} else {
			gtk_widget_set_style(window, form_fs);
			gtk_widget_set_style(top_toolbar, toolbar_fs);
			gtk_widget_set_style(pb_controls, toolbar_fs);
			gtk_widget_set_style(vol_controls, toolbar_fs);
			gtk_widget_set_style(time_label, label_fs);
			gtk_widget_set_style(volume_label, label_fs);
		}
		hide_controls();
		_fullscreen = TRUE;
	} else {
		_fullscreen = FALSE;  
		form_fs =  gtk_widget_get_style(window);
		label_fs = gtk_widget_get_style(time_label);
		toolbar_fs = gtk_widget_get_style(top_toolbar);
		gtk_window_unfullscreen((GtkWindow *)window);
		gtk_widget_set_style(window, form_orig);
		gtk_widget_set_style(top_toolbar, toolbar_orig);
		gtk_widget_set_style(pb_controls, toolbar_orig);
		gtk_widget_set_style(vol_controls, toolbar_orig);
		gtk_widget_set_style(time_label, label_orig);
		gtk_widget_set_style(volume_label, label_orig);
		show_controls();
	}
}

static void file_open_clicked( GtkWidget *widget, gpointer data ) {
	GtkWidget *dialog;
	opc_hidevideo();
	dialog = gtk_file_chooser_dialog_new (
			"Open File", 
			(GtkWindow *)window,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		set_video_path(filename);
		play_path();
		g_free (filename);
	}
	gtk_widget_destroy (dialog);
	opc_unhidevideo();
}

static gboolean window_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data) {
	if(widget == drawing_area) {
		da_x = event->x;
		da_y = event->y;
		da_w = event->width;
		da_h = event->height;
		calc_render_pos();
	} else if (widget == window) {
		window_x = event->x;
		window_y = event->y;
		gtk_widget_queue_resize ((GtkWidget *)drawing_area); 
		calc_render_pos();
	} 
	return FALSE;
}

static gboolean window_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	/* Block so that keys do not effect controls. */
	return TRUE;
}

static gboolean window_key_release_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	LOGD(TAG,"Key release '%d'",event->keyval);
	switch(event->keyval) {
		case 112:
			pause_clicked(NULL, NULL);
			break;
		case 65362:
			vol_up_clicked(NULL, NULL);
			break;
		case 65364:
			vol_down_clicked(NULL, NULL);
			break;
		case 65361:
			rewind_clicked(NULL, NULL);
			break;
		case 65363:
			ff_clicked(NULL, NULL);
			break;
		case 65366:
			next_clicked(NULL,NULL);
			break;
		case 65365:
			previous_clicked(NULL,NULL);
			break;
		case 65307:
			if(_fullscreen)
				fullscreen_clicked(NULL,NULL);
			break;
		case 65480:
			fullscreen_clicked(NULL,NULL);
			break;
	}
	return TRUE;
}

static void build_drawing_area(GtkBox *vbox) {
	GdkColor black;
	black.red = 0;
	black.green = 0;
	black.blue = 0;
	drawing_area = gtk_drawing_area_new();
	gtk_widget_modify_bg(drawing_area,GTK_STATE_NORMAL, &black);
	g_signal_connect((GtkObject *)drawing_area, "configure-event", G_CALLBACK(window_configure_event),NULL);
	gtk_box_pack_start((GtkBox *)vbox,drawing_area,TRUE,TRUE,0);
}

static void build_top_toolbar(GtkBox *vbox) {
	top_controls = gtk_hbox_new(0, 6);
	top_toolbar = gtk_toolbar_new();
	gtk_toolbar_set_show_arrow((GtkToolbar *)top_toolbar,FALSE);
	GtkToolItem *file_open = gtk_tool_button_new_from_stock("gtk-open");
	gtk_signal_connect((GtkObject *)file_open,"clicked",G_CALLBACK(file_open_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)top_toolbar,file_open, 0);

	GtkToolItem *fullscreen = gtk_toggle_tool_button_new_from_stock("gtk-fullscreen");
	gtk_signal_connect((GtkObject *)fullscreen,"clicked",G_CALLBACK(fullscreen_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)top_toolbar,fullscreen, 1);

	GtkToolItem *preferences = gtk_tool_button_new_from_stock("gtk-properties");
	gtk_signal_connect((GtkObject *)preferences,"clicked",G_CALLBACK(preferences_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)top_toolbar, preferences, 2);

	gtk_box_pack_start((GtkBox *)top_controls, top_toolbar, FALSE, FALSE, 0);
	gtk_box_pack_start((GtkBox *)vbox, top_controls, FALSE, FALSE, 0);
}

static void build_bottom_toolbar(GtkBox *vbox) {
	bottom_controls = gtk_hbox_new(0, 6);
	pb_controls = gtk_toolbar_new();

	gtk_toolbar_set_show_arrow((GtkToolbar *)pb_controls,FALSE);

	GtkToolItem *previous = gtk_tool_button_new_from_stock("gtk-media-previous");
	gtk_signal_connect((GtkObject *)previous,"clicked",G_CALLBACK(previous_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)pb_controls,previous, 0);

	GtkToolItem *rewind = gtk_tool_button_new_from_stock("gtk-media-rewind");
	gtk_signal_connect((GtkObject *)rewind,"clicked",G_CALLBACK(rewind_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)pb_controls,rewind, 1);

	GtkToolItem *pause = gtk_toggle_tool_button_new_from_stock("gtk-media-pause");
	gtk_signal_connect((GtkObject *)pause,"clicked",G_CALLBACK(pause_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)pb_controls,pause, 2);

	GtkToolItem *ff = gtk_tool_button_new_from_stock("gtk-media-forward");
	gtk_signal_connect((GtkObject *)ff,"clicked",G_CALLBACK(ff_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)pb_controls,ff, 3);

	GtkToolItem *next = gtk_tool_button_new_from_stock("gtk-media-next");
	gtk_signal_connect((GtkObject *)next,"clicked",G_CALLBACK(next_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)pb_controls,next, 4);

	gtk_box_pack_start((GtkBox *)bottom_controls, pb_controls, FALSE, FALSE, 0);

	gtk_box_pack_start((GtkBox *)bottom_controls, gtk_vseparator_new(), FALSE, FALSE, 0);

	time_label = gtk_label_new("00:00:00 / 00:00:00");
	gtk_box_pack_start((GtkBox *)bottom_controls, time_label, FALSE, FALSE, 0);

	gtk_box_pack_start((GtkBox *)bottom_controls, gtk_vseparator_new(), FALSE, FALSE, 0);

	hscale = gtk_hscale_new_with_range(0,100,1);
	gtk_scale_set_draw_value((GtkScale *)hscale,FALSE);
	gtk_signal_connect((GtkObject *)hscale,"change-value",G_CALLBACK(hscale_change_value),NULL);
	gtk_box_pack_start((GtkBox *)bottom_controls, hscale, TRUE, TRUE, 0);

	gtk_box_pack_start((GtkBox *)bottom_controls, gtk_vseparator_new(), FALSE, FALSE, 0);

	volume_label = gtk_label_new("Volume:");
	gtk_box_pack_start((GtkBox *)bottom_controls, volume_label, FALSE, FALSE, 0);

	vol_controls = gtk_toolbar_new();

	gtk_toolbar_set_show_arrow((GtkToolbar *)vol_controls,FALSE);

	GtkToolItem *vol_up = gtk_tool_button_new_from_stock("gtk-go-up");
	gtk_signal_connect((GtkObject *)vol_up,"clicked",G_CALLBACK(vol_up_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)vol_controls,vol_up, 0);

	GtkToolItem *vol_down = gtk_tool_button_new_from_stock("gtk-go-down");
	gtk_signal_connect((GtkObject *)vol_down,"clicked",G_CALLBACK(vol_down_clicked),NULL);
	gtk_toolbar_insert((GtkToolbar *)vol_controls,vol_down, 1);

	gtk_box_pack_end((GtkBox *)bottom_controls,vol_controls,FALSE,FALSE,0);

	gtk_box_pack_end((GtkBox *)vbox, bottom_controls, FALSE, FALSE, 0);
}

static void stretch_updated(void *setting) {
	setting_t *set = (setting_t *)setting;
	opc_set_aspect(set->int_value ? "stretch":"Letterbox");
	LOGD(TAG, "%s", "stretch updated");
}

static void init_settings() {
	settings_init();
	settings_read(&cont_pb);
	settings_read(&volume);
	stretch.setting_update_cb = &stretch_updated;
	settings_read(&stretch);
	settings_read(&audio_out);
	audio_settings = list_create("Audio",2,1);
	list_add_entry(&audio_settings,&volume);
	list_add_entry(&audio_settings, &audio_out);
	playback_settings = list_create("Playback",2,1);
	list_add_entry(&playback_settings, &cont_pb);
	list_add_entry(&playback_settings, &stretch);
}

int main (int argc, char * argv[]) {
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
	gtk_init(&argc ,&argv);
	gdk_threads_init();
	init_settings();
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title((GtkWindow *)window,"tomxplayer"); 
	gtk_window_set_icon((GtkWindow *)window,gdk_pixbuf_new_from_file("tomxplayer.png", &gerr));
	gtk_window_stick((GtkWindow *)window);
	gtk_window_set_keep_above((GtkWindow *)window,TRUE);
	gtk_widget_set_events((GtkWidget *)window, GDK_ALL_EVENTS_MASK);
	g_signal_connect((GtkObject *)window,"destroy",G_CALLBACK(destroy), NULL);
	g_signal_connect((GtkObject *)window, "configure-event", G_CALLBACK(window_configure_event),NULL);
	g_signal_connect((GtkObject *)window, "window-state-event", G_CALLBACK(event_window_state),NULL);
	g_signal_connect((GtkObject *)window, "motion-notify-event", G_CALLBACK(window_motion_notify_event),NULL);
	g_signal_connect((GtkObject *)window, "key-press-event", G_CALLBACK(window_key_press_event),NULL);
	g_signal_connect((GtkObject *)window, "key-release-event", G_CALLBACK(window_key_release_event),NULL);
	gtk_window_set_default_size((GtkWindow *)window, 640, 360);
	//gtk_container_set_resize_mode((GtkContainer *)window,GTK_RESIZE_IMMEDIATE);
	GtkWidget *vbox = gtk_vbox_new(0, 0); 
	gtk_container_add((GtkContainer *)window,vbox);
	build_top_toolbar((GtkBox *)vbox);
	build_drawing_area((GtkBox *)vbox); 
	build_bottom_toolbar((GtkBox *)vbox);
	gtk_widget_show_all(window);

	if(argc > 1) {
		if(set_video_path(argv[argc -1]))
			play_path();
	}
	gtk_main();
	opc_stop_omxplayer();
	return 0;
}
