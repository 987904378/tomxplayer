//
//  setting_treeview.h
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
#ifndef SETTING_TREEVIEW_H
#define SETTING_TREEVIEW_H
#include <gtk/gtk.h>
#include "list.h"

typedef struct {
	GtkWidget *this;
	GtkTreeViewColumn *column;
	GtkWidget *treeview;
	GtkTreeStore *model;
	GtkCellRenderer *cell_renderer;
} settings_treeview_t;

settings_treeview_t *gtk_settings_treeview_new();
void gtk_settings_treeview_add(settings_treeview_t *setting_treeview, list_t *setting_list) ;

#endif
