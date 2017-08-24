//
//  setting_list_view.h
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

#ifndef SETTING_LIST_VIEW_H
#define SETTING_LIST_VIEW_H
#include <gtk/gtk.h>
#include "settings.h"
#include "list.h"

typedef struct {
	GtkWidget *this;
	GtkWidget *vbox;
	list_t vbox_children;
} setting_list_view_t;

setting_list_view_t *gtk_setting_list_view_new();
void gtk_setting_list_view_free(setting_list_view_t *slv);
void gtk_setting_list_view_add(setting_list_view_t * slv, list_t *settings);

#endif