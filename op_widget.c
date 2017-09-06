//
//  op_widget.c
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
#include "op_widget.h"
#include "main_settings.h"
#ifndef NO_OSD
#include "osd_rpi/text_render.h"
#endif
#include "log.h"
#include <string.h>
#include "op_dbus.h"

#define TAG "op_widget"


#ifdef GTK3
static GtkCssProvider *fs_css_provider;
static const char *fs_css = "GtkDrawingArea { background: #000000; color: #FFFFFF; }";
#endif

static void calc_render_pos(op_widget_t *temp) {
	temp->pos[0] = temp->window_x + temp->da_x;
	temp->pos[1] = temp->window_y + temp->da_y;
	temp->pos[2] = temp->pos[0] + temp->da_w;
	temp->pos[3] = temp->pos[1] + temp->da_h;
#ifdef POLLWINPOS
	if(!temp->fullscreen) {
		if(!temp->maximized) {
			temp->pos[0] += border_offset.int_value;
			temp->pos[1] += border_offset.int_value;
			temp->pos[2] += border_offset.int_value;
			temp->pos[3] += border_offset.int_value;
		}
		temp->pos[1] += title_bar_offset.int_value;
		temp->pos[3] += title_bar_offset.int_value;
	}
#endif
#ifndef NO_OSD
	tr_set_xy(temp->tr, (unsigned int)temp->pos[0] + 10,(unsigned int)temp->pos[1] +10);
	tr_set_max_width(temp->tr, (unsigned int)temp->da_w - 20);
	tr_set_max_height(temp->tr, (unsigned int)temp->da_h - 20);
	if(osd_textsize_percent.int_value) {
		double percent = osd_textsize.int_value * 0.01;
		tr_set_text_size(temp->tr, (unsigned int)(temp->da_h * percent));
	}
#endif
	opc_update_pos(temp->pos); 
}

static gboolean window_configure_event(GtkWidget *widget, GdkEventConfigure *event, gpointer user_data) {
	op_widget_t *temp = (op_widget_t *)user_data;
	if(widget == temp->drawing_area) {
		temp->da_x = event->x;
		temp->da_y = event->y;
		temp->da_w = event->width;
		temp->da_h = event->height;
		LOGD(TAG, "%s", "drawing area size set");
		calc_render_pos(temp);
	} else if (widget == temp->window) {
		temp->window_x = event->x;
		temp->window_y = event->y;
		temp->window_w = event->width;
		temp->window_h = event->height;
		LOGD(TAG, "%s", "window coords set");
		gtk_widget_queue_resize ((GtkWidget *)temp->drawing_area);
#if GTK3
		temp->da_w = event->width < temp->da_w ? event->width - temp->diff_w : temp->da_w;
		temp->da_h = event->height < temp->da_h ? event->height - temp->diff_h : temp->da_h;
#endif
		calc_render_pos(temp);
	} 
	return FALSE;
}

static gboolean event_window_state (GtkWidget *widget, GdkEventWindowState *event, gpointer user_data) {
	op_widget_t *temp = (op_widget_t *)user_data;
	if(event->new_window_state & GDK_WINDOW_STATE_ICONIFIED) {
		temp->minimized = TRUE;
		opc_hidevideo();
#ifndef NO_OSD
		tr_stop(temp->tr);
#endif
	} else { 
		temp->minimized = FALSE;
		opc_unhidevideo();
	}
	temp->maximized = (event->new_window_state & GDK_WINDOW_STATE_MAXIMIZED);
	temp->fullscreen = (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN);
	return FALSE;
}

#ifdef POLLWINPOS
static gboolean window_position_poll(gpointer arg) {
	op_widget_t *temp = (op_widget_t *)arg;
	if(temp->window) {
		int x, y;
		gtk_window_get_position ((GtkWindow *)temp->window, &x, &y);
		if(x != temp->window_x || y != temp->window_y) {
			temp->window_x = x;
			temp->window_y = y;
			calc_render_pos(temp);
		}
	}
	return TRUE;
}
#endif

static void calc_window_da_diff(op_widget_t *temp) {
	temp->diff_w = temp->window_w - temp->da_w;
	temp->diff_h = temp->window_h - temp->da_h;
}

int op_widget_status(long long pbpos[]) {
	return opc_status(pbpos);
}

void op_widget_toggle_playpause() {
	opc_toggle_playpause();
}

void op_widget_stop_omxplayer(op_widget_t *opw) {
#ifndef NO_OSD
	tr_stop(opw->tr);
#endif
	opc_stop_omxplayer();
}

int op_widget_set_volume(double vol) {
	return opc_set_volume(vol);
}

void op_widget_set_pb_position(unsigned long pos) {
	opc_set_pb_position(pos);
}

void op_widget_hidevideo(op_widget_t *opw) {
#ifndef NO_OSD
	tr_stop(opw->tr);
#endif
	opc_hidevideo();
}

void op_widget_unhidevideo(op_widget_t *opw) {
	opc_unhidevideo();
}

void op_widget_set_aspect(char * aspect) {
	opc_set_aspect(aspect);
}

void op_widget_set_alpha(int alpha) {
	opc_set_alpha(alpha);
}

int op_widget_is_running() {
	return opc_is_running();
}

void op_widget_play(op_widget_t *op_widget, char *vpath) {
	calc_window_da_diff(op_widget);
	op_widget->file_name = vpath;
	opc_start_omxplayer_thread(op_widget->pos, vpath);
}

void op_widget_osd_show(op_widget_t *opw, char *text) {
#ifndef NO_OSD
	if(!opw->minimized) {
		tr_set_text(opw->tr, text);
		tr_show_thread(opw->tr);
	}
#endif
}

void op_widget_osd_set_text_size(op_widget_t *opw, unsigned int size) {
#ifndef NO_OSD
	tr_set_text_size(opw->tr, size);
	LOGD(TAG, "%s", "osd_text_size updated");
#endif
}

void op_widget_osd_set_text_size_percent(op_widget_t *opw, double percent) {
#ifndef NO_OSD
	op_widget_osd_set_text_size(opw, (unsigned int)(opw->da_h * percent));
	LOGD(TAG, "%s", "osd_text_size updated");
#endif
}

int op_widget_is_ready(op_widget_t *opw) {
	if(opw != NULL && opw->da_w > 0 && opw->da_h > 0) {
		return 1;
	} else
		return 0;
}

void op_widget_destroy(op_widget_t *opw) {
#ifndef NO_OSD
	tr_stop(opw->tr);
	op_widget_stop_omxplayer(opw);
	//tr_deinit();
#else
	op_widget_stop_omxplayer(opw);
#endif
}

static void arb_offset_updated(void *setting, void *user_data) {
	calc_render_pos((op_widget_t *)user_data);
}

op_widget_t *op_widget_new(GtkWindow *window) {
	op_widget_t *temp = malloc(sizeof(op_widget_t));
	temp->drawing_area = gtk_drawing_area_new();
	temp->window = (GtkWidget *)window;
#ifdef GTK3
	GError *gerr = NULL;
	fs_css_provider = gtk_css_provider_new();
	gtk_css_provider_load_from_data (fs_css_provider, fs_css, strlen(fs_css), &gerr);
	gtk_style_context_add_provider
			(gtk_widget_get_style_context((GtkWidget *)temp->drawing_area),(GtkStyleProvider *)fs_css_provider, GTK_STYLE_PROVIDER_PRIORITY_USER);
#else
	GdkColor black;
	black.red = 0;
	black.green = 0;
	black.blue = 0;
	gtk_widget_modify_bg(temp->drawing_area,GTK_STATE_NORMAL, &black);
#endif
	gtk_window_set_keep_above((GtkWindow *)window,TRUE);
	gtk_widget_set_events((GtkWidget *)window, GDK_ALL_EVENTS_MASK);
	g_signal_connect((GObject *)temp->drawing_area, "configure-event", G_CALLBACK(window_configure_event),temp);
#ifndef POLLWINPOS
	g_signal_connect((GObject *)window, "configure-event", G_CALLBACK(window_configure_event),temp);
	gtk_window_set_position ((GtkWindow *)window, GTK_WIN_POS_CENTER);
#else
	gdk_threads_add_timeout(20, &window_position_poll, temp);
#endif
	g_signal_connect((GObject *)window, "window-state-event", G_CALLBACK(event_window_state),temp);
#ifndef NO_OSD
	temp->tr = tr_new();
#endif
	arb_x_offset.setting_update_cb = &arb_offset_updated;
	arb_x_offset.setting_update_cb_user_data = temp;
	arb_y_offset.setting_update_cb = &arb_offset_updated;
	arb_y_offset.setting_update_cb_user_data = temp;
	temp->window_y = 0;
	temp->window_x = 0;
	temp->window_w = 0;
	temp->window_h = 0;
	temp->da_x = 0;
	temp->da_y = 0;
	temp->da_w = 0;
	temp->da_h = 0;
	temp->diff_w = 0;
	temp->diff_h = 0;
	temp->pos[0] = 0;
	temp->pos[1] = 0;
	temp->pos[2] = 0;
	temp->pos[3] = 0;
	temp->minimized = FALSE;
	temp->maximized = FALSE;
	temp->fullscreen = FALSE;
	temp->is_running = 0;
	temp->hidden = 0;
	temp->thread = 0;

	opc_init();

	return temp;
}
