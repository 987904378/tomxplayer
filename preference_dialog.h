//
//  preference_dialog.h
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
#ifndef PREFERENCE_DIALOG_H
#define PREFERENCE_DIALOG_H
#include "settings.h"
#include "setting_treeview.h"
#include <gtk/gtk.h>
#include "list.h"
#include "setting_list_view.h"

typedef struct {
	GtkWidget *window;
	settings_treeview_t *settings_treeview;
	setting_list_view_t *setting_list_view;
} preference_dialog_t;

preference_dialog_t *gtk_preference_dialog_new(GtkWindow *parent);
void gtk_preference_dialog_free(preference_dialog_t *pd);
void gtk_preference_dialog_add(preference_dialog_t *pd, list_t *setting_list);

#endif 
