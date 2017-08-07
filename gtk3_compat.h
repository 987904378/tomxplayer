//
//  gtk3_compat.h
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
#define gtk_vbox_new(x, y) gtk_box_new(1,y);
#define gtk_hbox_new(x, y) gtk_box_new(0,y);
#define gtk_tool_button_new_from_stock new_stock_id_tool_button
#define gtk_toggle_tool_button_new_from_stock new_stock_id_toggle_tool_button
#define gtk_hseparator_new() gtk_separator_new (0)
#define gtk_vseparator_new() gtk_separator_new (1)
#define gtk_hscale_new_with_range(x, y, z) gtk_scale_new_with_range(0, x, y, z)
#define gtk_scrolled_window_add_with_viewport gtk_container_add
#define gtk_widget_modify_bg modify_background_color

static inline GtkToolItem *new_stock_id_tool_button(char * stock_id) {
	return gtk_tool_button_new(gtk_image_new_from_icon_name(stock_id, GTK_ICON_SIZE_LARGE_TOOLBAR), stock_id);
}

static inline GtkToolItem *new_stock_id_toggle_tool_button(char * stock_id) {
	GtkToolItem *button = gtk_toggle_tool_button_new();
	gtk_tool_button_set_icon_widget ((GtkToolButton *)button, gtk_image_new_from_icon_name(stock_id, GTK_ICON_SIZE_LARGE_TOOLBAR));
	return button;
}

static inline void modify_background_color(GtkWidget *widget, GtkStateFlags state, GdkColor *color) {
	GdkRGBA rgba;
	rgba.red = (double)color->red / 65536;
	rgba.blue = (double)color->blue / 65536;
	rgba.green = (double)color->green / 65536;
	rgba.alpha = 1.0;
	gtk_widget_override_background_color(widget, state, &rgba);
}

