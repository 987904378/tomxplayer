//
//  top_widget.c
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
#include "top_widget.h"
#include "op_widget.h"
#include "main_settings.h"
#include "time_utils.h"
#include <errno.h>
#include "log.h"
#include <string.h>
#ifdef GTK3
#include "gtk3_compat.h"
#endif
#include <libgen.h>

#define TAG "top_widget"

static int do_volume(int vol) {
	int ret = 0;
	double dvol = (double)vol / 100;
	ret = op_widget_set_volume(dvol);
	return ret;
}

static void * restore_volume(void * arg) {
	pthread_detach(pthread_self());
	while(do_volume(volume.int_value))
		usleep(100 * 1000);
	top_widget_t *topw = (top_widget_t *)arg;
	if(!topw->op_widget->minimized)
		op_widget_unhidevideo();
	return NULL;
}

static void update_pb_position_ui(top_widget_t *topw) {
	char dur[255];
	char pos[255];
	char timestamp[255];
	if(topw->pb_dur <= 0) return;
	gtk_range_set_range((GtkRange *)topw->hscale, 0, (gdouble)topw->pb_dur);
	gtk_range_set_value((GtkRange *)topw->hscale, (gdouble)topw->pb_pos);
	if(!ms_to_time(topw->pb_dur,dur) && !ms_to_time(topw->pb_pos,pos)) {
		sprintf(timestamp,"%s / %s",pos,dur);
		gtk_label_set_text((GtkLabel *)topw->time_label,timestamp);
	}
}

#ifdef GTK3
static gboolean update_pb_position_ui_gtk3(gpointer arg) {
	update_pb_position_ui((top_widget_t *)arg);
	return FALSE;
}
#endif

static void *pb_pos_poll(void * arg) {
	LOGD(TAG, "%s", "pos_poll_started");
	top_widget_t *topw = (top_widget_t *)arg;
	long long pb_pos[2];
	pthread_detach(pthread_self());
	topw->pb_pos_poll_running = 1; 
	while(!topw->pb_pos_poll_cancel) {
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
		usleep(950 * 1000);
		if(!op_widget_is_running() && cont_pb.int_value && !topw->pb_pos_poll_cancel) {
			mp_move_next(topw->playlist);
			top_widget_play_path(topw);
		}
		if(topw->pb_pos_poll_cancel)
			break;
		if(op_widget_status(pb_pos) || !topw->should_uslider || !op_widget_is_running())
			continue;
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
		topw->pb_pos = pb_pos[1];
		topw->pb_dur = pb_pos[0];
#ifdef GTK3
		g_main_context_invoke(topw->context, &update_pb_position_ui_gtk3, topw);
#else
		gdk_threads_enter();
		update_pb_position_ui(topw);
		gdk_threads_leave();
#endif
	}
	topw->pb_pos_poll_running = 0;
	return NULL;
}


static void * timed_set_pb_position(void *arg) {
    int c = 0;
	pthread_detach(pthread_self());
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	while(c < 499) {
		usleep(1000);
		c++;
	}
	top_widget_t *topw = (top_widget_t *)arg;
	unsigned long pb_pos = (unsigned long)topw->pb_pos;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
	op_widget_set_pb_position(pb_pos);
	topw->should_uslider = TRUE;
	return NULL;
}

static gboolean hscale_change_value(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data) {
	top_widget_t *topw = (top_widget_t *)user_data;
	GtkAdjustment *adj = gtk_range_get_adjustment((GtkRange *)topw->hscale);
#ifdef GTK3
	double upper = gtk_adjustment_get_upper(adj);
	/* Work around for hscale issue which allows you to drag
	 * slider to value greater than upper?
	 */
	upper -= (2000 * 1000); /* Can't seek to the very end without omxplayer error 256. */
	value = value > upper ? upper : value;
	long long newval = (long long)value;
#else
	long long newval = (long long)adj->value;
#endif
	topw->should_uslider = FALSE;
	topw->pb_pos = newval;
	pthread_cancel(topw->set_pb_position_thread);
	pthread_create(&topw->set_pb_position_thread, NULL, &timed_set_pb_position, topw);
#ifdef GTK3
	topw->pb_dur = (long long)gtk_adjustment_get_upper(adj);
	update_pb_position_ui(topw);
#ifndef NO_OSD
	char *time = strdup(gtk_label_get_text((GtkLabel *)topw->time_label));
	op_widget_osd_show(topw->op_widget, time);
#endif
#else
	topw->pb_dur = (long long)adj->upper;
	update_pb_position_ui(topw);
#ifndef NO_OSD
	op_widget_osd_show(topw->op_widget, ((GtkLabel *)topw->time_label)->label);
#endif
#endif
	return FALSE;
}

static gboolean ff_clicked( GtkWidget *widget, gpointer data ) {
	top_widget_t *topw = (top_widget_t *)data;
	GtkAdjustment *adj = gtk_range_get_adjustment((GtkRange *)topw->hscale);
	topw->should_uslider = FALSE;
#ifdef GTK3
	long long value = (long long)gtk_adjustment_get_value (adj);
	value += 10 * 1000 * 1000;
	double upper = gtk_adjustment_get_upper(adj);
	/* Work around for hscale issue which allows you to drag
	 * slider to value greater than upper?
	 */
	upper -= (2000 * 1000); /* Can't seek to the very end without omxplayer error 256. */
	value = value > upper ? upper : value;
	op_widget_set_pb_position(value);
	topw->pb_pos = (long long)value;
	topw->pb_dur = (long long)gtk_adjustment_get_upper(adj);
	update_pb_position_ui(topw);
#ifndef NO_OSD
	char *time = strdup(gtk_label_get_text((GtkLabel *)topw->time_label));
	op_widget_osd_show(topw->op_widget, time);
#endif
#else
	adj->value += 10 * 1000 * 1000;
	op_widget_set_pb_position(adj->value);
	topw->pb_pos = (long long)adj->value;
	topw->pb_dur = (long long)adj->upper;
	update_pb_position_ui(topw);
#ifndef NO_OSD
	op_widget_osd_show(topw->op_widget, ((GtkLabel *)topw->time_label)->label);
#endif
#endif
	topw->should_uslider = TRUE;
	return FALSE;
}

static gboolean rewind_clicked( GtkWidget *widget, gpointer data ) {
	top_widget_t *topw = (top_widget_t *)data;
	GtkAdjustment *adj = gtk_range_get_adjustment((GtkRange *)topw->hscale);
	topw->should_uslider = FALSE;
#ifdef GTK3
	long long value = (long long)gtk_adjustment_get_value (adj);
	value -= 10 * 1000 * 1000;
	double lower = gtk_adjustment_get_lower(adj);
	value = value < lower ? lower : value;
	op_widget_set_pb_position(value);
	topw->pb_pos = (long long)value;
	topw->pb_dur = (long long)gtk_adjustment_get_upper(adj);
	update_pb_position_ui(topw);
#ifndef NO_OSD
	char *time = strdup(gtk_label_get_text((GtkLabel *)topw->time_label));
	op_widget_osd_show(topw->op_widget, time);
#endif
#else
	adj->value -= 10 * 1000 * 1000;
	op_widget_set_pb_position(adj->value);
	topw->pb_pos = (long long)adj->value;
	topw->pb_dur = (long long)adj->upper;
	update_pb_position_ui(topw);
#ifndef NO_OSD
	op_widget_osd_show(topw->op_widget, ((GtkLabel *)topw->time_label)->label);
#endif
#endif
	topw->should_uslider = TRUE;
	return FALSE;
}

static gboolean vol_up_clicked( GtkWidget *widget, gpointer data ) {
	top_widget_t *topw = (top_widget_t *)data;
	volume.int_value += 5;
	volume.int_value = volume.int_value > volume.max ? volume.max : volume.int_value;
	do_volume(volume.int_value);
	settings_save(&volume);
#ifndef NO_OSD
	char osd_text[255];
	sprintf(osd_text, "VOL: %d",volume.int_value);
	op_widget_osd_show(topw->op_widget, osd_text);
#endif
	return FALSE;
}

static gboolean vol_down_clicked( GtkWidget *widget, gpointer data ) {
	top_widget_t *topw = (top_widget_t *)data;
	volume.int_value -= 5;
	volume.int_value = volume.int_value < volume.min ? volume.min : volume.int_value;
	do_volume(volume.int_value);
	settings_save(&volume);
#ifndef NO_OSD
	char osd_text[255];
	sprintf(osd_text, "VOL: %d",volume.int_value);
	op_widget_osd_show(topw->op_widget, osd_text);
#endif
	return FALSE;
}

static void pause_clicked( GtkWidget *widget, gpointer data ) {
	top_widget_t *topw = (top_widget_t *)data;
	topw->paused = topw->paused ? FALSE : TRUE;
#ifndef NO_OSD
	op_widget_osd_show(topw->op_widget, topw->paused ? "Paused" : "Playing");
#endif
	op_widget_toggle_playpause();
}

static void previous_clicked( GtkWidget *widget, gpointer data ) {
	top_widget_t *topw = (top_widget_t *)data;
	if(cont_pb.int_value) {
		unsigned long pos = (unsigned long) gtk_range_get_value((GtkRange *)topw->hscale);
		if(pos > (1000 * 5000))
			op_widget_set_pb_position(0);
		else {
			mp_move_previous(topw->playlist);
			top_widget_play_path(topw);
		}
	} else {
		op_widget_set_pb_position(0);
	}
}

static void next_clicked( GtkWidget *widget, gpointer data ) {
	top_widget_t *topw = (top_widget_t *)data;
	if(cont_pb.int_value) {
		mp_move_next(topw->playlist);
		top_widget_play_path(topw);
	}
}

int top_widget_set_video_path(top_widget_t *topw, char *path) {
	int is_http = !strncmp(path,"http://",7) || !strncmp(path,"https://",8);
	FILE * fd = fopen(path,"r");
	if(fd == NULL && !is_http) {
		int err = errno;
		LOGE(TAG, "path '%s' - %s", path, strerror(err));
		return err;
	}
	LOGD(TAG, "Video path set to %s",path);
	if(topw->playlist != NULL)
		mp_free(topw->playlist);
	if(cont_pb.int_value && !is_http)
		topw->playlist = mp_create_dir_of_file(path);
	else {
		topw->playlist = mp_create();
		mp_add(topw->playlist, path);
	}
	return 0;
}

void top_widget_play_path(top_widget_t *topw) {
	char vpath[255];
	if(!topw->playlist) {
		LOGE(TAG,"%s", "_playlist is NULL?");
		return;
	}
	mp_get_current(topw->playlist, vpath);
	op_widget_play(topw->op_widget, vpath);
	if(!topw->pb_pos_poll_running) {
		pthread_create(&topw->pb_pos_poll_thread,NULL, &pb_pos_poll,topw); 
	}
	//sleep(1);
	pthread_create(&topw->restore_volume_thread,NULL, &restore_volume,topw);
#ifndef NO_OSD
	op_widget_osd_show(topw->op_widget, basename(vpath));
#endif
}

void top_widget_toggle_playpause(top_widget_t *topw) {
	gtk_toggle_tool_button_set_active
		((GtkToggleToolButton *)topw->pause, topw->paused ? FALSE : TRUE);
}

void top_widget_stop(top_widget_t *topw) {
	op_widget_stop_omxplayer();
	topw->pb_pos = 0;
	topw->pb_dur = 100;
	update_pb_position_ui(topw);
	topw->pb_pos_poll_cancel = 1;
	pthread_cancel(topw->pb_pos_poll_thread);
	pthread_join(topw->pb_pos_poll_thread, NULL);
	topw->pb_pos_poll_running = 0;
	topw->pb_pos_poll_cancel = 0;
}

void top_widget_next(top_widget_t *topw) {
	next_clicked(NULL , topw);
}

void top_widget_previous(top_widget_t *topw) {
	previous_clicked(NULL , topw);
}

void top_widget_seek_forward(top_widget_t *topw) {
	ff_clicked(NULL , topw);
}

void top_widget_seek_back(top_widget_t *topw) {
	rewind_clicked(NULL , topw);
}

void top_widget_volume_up(top_widget_t *topw) {
	vol_up_clicked(NULL , topw);
}

void top_widget_volume_down(top_widget_t *topw) {
	vol_down_clicked(NULL , topw);
}

void top_widget_hidecontrols(top_widget_t *topw) {
	gtk_widget_hide(topw->bottom_controls);
	gtk_widget_hide(topw->hscale);
}

void top_widget_showcontrols(top_widget_t *topw) {
	gtk_widget_show(topw->bottom_controls);
	gtk_widget_show(topw->hscale);
}

void top_widget_destroy(top_widget_t *topw) {
	topw->pb_pos_poll_cancel = 1;
	pthread_cancel(topw->pb_pos_poll_thread);
	pthread_join(topw->pb_pos_poll_thread, NULL);
	op_widget_destroy(topw->op_widget);
	free(topw);
}

top_widget_t *top_widget_new(GtkWindow *window) {
	top_widget_t *temp = malloc(sizeof(top_widget_t));
	temp->widget = gtk_vbox_new(FALSE, 0);
	temp->bottom_controls = gtk_hbox_new(0, 6);
	temp->op_widget = op_widget_new(window);
	gtk_box_pack_start((GtkBox *)temp->widget,temp->op_widget->drawing_area,TRUE,TRUE,0);

	temp->pb_controls = gtk_toolbar_new();
	gtk_toolbar_set_show_arrow((GtkToolbar *)temp->pb_controls,FALSE);

	GtkToolItem *previous = gtk_tool_button_new_from_stock("gtk-media-previous");
	g_signal_connect((GObject *)previous,"clicked",G_CALLBACK(previous_clicked), temp);
	gtk_toolbar_insert((GtkToolbar *)temp->pb_controls, previous, 0);

	GtkToolItem *rewind = gtk_tool_button_new_from_stock("gtk-media-rewind");
	g_signal_connect((GObject *)rewind,"clicked",G_CALLBACK(rewind_clicked), temp);
	gtk_toolbar_insert((GtkToolbar *)temp->pb_controls, rewind, 1);

	temp->pause = gtk_toggle_tool_button_new_from_stock("gtk-media-pause");
	g_signal_connect((GObject *)temp->pause,"clicked",G_CALLBACK(pause_clicked), temp);
	gtk_toolbar_insert((GtkToolbar *)temp->pb_controls, temp->pause, 2);

	GtkToolItem *ff = gtk_tool_button_new_from_stock("gtk-media-forward");
	g_signal_connect((GObject *)ff,"clicked",G_CALLBACK(ff_clicked), temp);
	gtk_toolbar_insert((GtkToolbar *)temp->pb_controls,ff, 3);

	GtkToolItem *next = gtk_tool_button_new_from_stock("gtk-media-next");
	g_signal_connect((GObject *)next,"clicked",G_CALLBACK(next_clicked), temp);
	gtk_toolbar_insert((GtkToolbar *)temp->pb_controls,next, 4);

	gtk_box_pack_start((GtkBox *)temp->bottom_controls, temp->pb_controls, TRUE, TRUE, 0);

	gtk_box_pack_start((GtkBox *)temp->bottom_controls, gtk_vseparator_new(), FALSE, FALSE, 0);

	temp->time_label = gtk_label_new("00:00:00 / 00:00:00");
	gtk_box_pack_start((GtkBox *)temp->bottom_controls, temp->time_label, FALSE, FALSE, 0);

	gtk_box_pack_start((GtkBox *)temp->bottom_controls, gtk_vseparator_new(), FALSE, FALSE, 0);

	temp->hscale = gtk_hscale_new_with_range(0,100,1);
	gtk_scale_set_draw_value((GtkScale *)temp->hscale,FALSE);
	g_signal_connect((GObject *)temp->hscale,"change-value",G_CALLBACK(hscale_change_value), temp);
#ifdef GTK3
	gtk_widget_set_margin_end (temp->hscale, 10);
	gtk_widget_set_margin_start (temp->hscale, 10);
#endif
	gtk_box_pack_start((GtkBox *)temp->widget, temp->hscale, FALSE, FALSE, 0);

	temp->vol_controls = gtk_toolbar_new();

	gtk_toolbar_set_show_arrow((GtkToolbar *)temp->vol_controls, FALSE);

	GtkToolItem *vol_up = gtk_tool_button_new_from_stock("gtk-go-up");
	g_signal_connect((GObject *)vol_up,"clicked",G_CALLBACK(vol_up_clicked), temp);
	gtk_toolbar_insert((GtkToolbar *)temp->vol_controls,vol_up, 0);

	GtkToolItem *vol_down = gtk_tool_button_new_from_stock("gtk-go-down");
	g_signal_connect((GObject *)vol_down,"clicked",G_CALLBACK(vol_down_clicked), temp);
	gtk_toolbar_insert((GtkToolbar *)temp->vol_controls,vol_down, 1);

	gtk_box_pack_end((GtkBox *)temp->bottom_controls,temp->vol_controls,FALSE,FALSE,0);

	temp->volume_label = gtk_label_new("Volume:");
	gtk_box_pack_end((GtkBox *)temp->bottom_controls, temp->volume_label, FALSE, FALSE, 0);

	gtk_box_pack_end((GtkBox *)temp->widget, temp->bottom_controls, FALSE, FALSE, 0);

	temp->playlist = NULL;
	temp->paused = FALSE;
	temp->should_uslider = TRUE;
	temp->pb_pos = 0;
	temp->pb_dur = 0;
	temp->pb_pos_poll_cancel = 0;
	temp->pb_pos_poll_running = 0;
	temp->pb_pos_poll_thread = 0;
	temp->restore_volume_thread = 0;
	temp->set_pb_position_thread = 0;

#ifdef GTK3
	temp->context = g_main_context_default();
#endif

	return temp;
}