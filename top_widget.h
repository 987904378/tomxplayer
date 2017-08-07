//
//  top_widget.h
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
//  along with this program.  If not, see <http://www.gnu.org/licenses/>
#ifndef TOP_WIDGET_H
#define TOP_WIDGET_H

#include <gtk/gtk.h>
#include "op_widget.h"
#include "media_playlist.h"

typedef struct {
	GtkWidget *widget;
	op_widget_t *op_widget;
	GtkWidget *bottom_controls;
	GtkWidget *pb_controls;
	GtkToolItem *pause;
	GtkWidget *time_label;
	GtkWidget *hscale;
	GtkWidget *volume_label;
	GtkWidget *vol_controls;
	media_playlist_t * playlist;
#ifdef GTK3
	GMainContext *context;
#endif
	gboolean paused;
	gboolean should_uslider;
	int64_t pb_pos;
	int64_t pb_dur;
	pthread_t pb_pos_poll_thread;
	int pb_pos_poll_cancel;
	int pb_pos_poll_running;
	pthread_t restore_volume_thread;
	pthread_t set_pb_position_thread;
	int alpha;
} top_widget_t;

#define top_widget_hidevideo(x) op_widget_hidevideo(x->op_widget)
#define top_widget_unhidevideo(x) op_widget_unhidevideo(x->op_widget)
#define top_widget_is_ready(x) op_widget_is_ready(x->op_widget)
#define top_widget_osd_show(x, y) op_widget_osd_show(x->op_widget, y)
#define top_widget_osd_set_text_size_percent(x, y) \
	op_widget_osd_set_text_size_percent(x->op_widget, y)
#define top_widget_osd_set_text_size(x, y) \
	op_widget_osd_set_text_size(x->op_widget, y)

top_widget_t *top_widget_new(GtkWindow *window);
int top_widget_set_video_path(top_widget_t *topw, char *path);
void top_widget_play_path(top_widget_t *topw);
void top_widget_showcontrols(top_widget_t *topw);
void top_widget_hidecontrols(top_widget_t *topw);
void top_widget_destroy(top_widget_t *topw);
void top_widget_volume_up(top_widget_t *topw);
void top_widget_volume_down(top_widget_t *topw);
void top_widget_seek_forward(top_widget_t *topw);
void top_widget_seek_back(top_widget_t *topw);
void top_widget_next(top_widget_t *topw);
void top_widget_previous(top_widget_t *topw);
void top_widget_stop(top_widget_t *topw);
void top_widget_toggle_playpause(top_widget_t *topw);
void top_widget_set_alpha(top_widget_t *topw, int alpha);

#endif