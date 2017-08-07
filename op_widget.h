//
//  op_widget.h
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
#ifndef OP_WIDGET_H
#define OP_WIDGET_H

#include <gtk/gtk.h>
#include <pthread.h>
#include "op_control.h"

#ifndef NO_OSD
#include "osd_rpi/text_render.h"
#endif

typedef struct {
	GtkWidget *drawing_area;
	GtkWidget *window;
	int window_x;
	int window_y;
	int window_w;
	int window_h;
	int da_x;
	int da_y;
	int da_w;
	int da_h;
	int diff_w;
	int diff_h;
	int pos[4];
	gboolean minimized;
	gboolean maximized;
	gboolean fullscreen;
	char *file_name;
	char *aspect;
	int is_running;
	int hidden;
	pthread_t thread;
#ifndef NO_OSD
	text_render_t *tr;
#endif
} op_widget_t;

void op_widget_set_pb_position(int64_t *ms);
int op_widget_status(int64_t pbpos[]);
void op_widget_hidevideo(op_widget_t *opw);
void op_widget_unhidevideo(op_widget_t *opw);
void op_widget_toggle_playpause();
void op_widget_stop_omxplayer(op_widget_t *opw);
int op_widget_set_volume(double vol);
void op_widget_set_aspect(char *aspect);
void op_widget_set_alpha(int alpha);
int op_widget_is_running();

void op_widget_hidecontrols(op_widget_t *opw);
void op_widget_destroy(op_widget_t *opw);
int op_widget_is_ready(op_widget_t *opw);
void op_widget_osd_show(op_widget_t *op_widget, char *text);
void op_widget_osd_set_text_size(op_widget_t *opw, unsigned int size);
void op_widget_osd_set_text_size_percent(op_widget_t *opw, double percent);
void op_widget_play(op_widget_t *op_widget, char *vpath);
op_widget_t *op_widget_new(GtkWindow *window);

#endif