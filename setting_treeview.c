//
//  setting_treeview.c
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
#include <stdlib.h>
#include "settings.h"
#include "log.h"
#include "setting_treeview.h"

settings_treeview_t *gtk_settings_treeview_new() {
	settings_treeview_t *temp = malloc(sizeof(settings_treeview_t));
	temp->this = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow *)temp->this,GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
	temp->model = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_POINTER);
	temp->treeview = gtk_tree_view_new_with_model((GtkTreeModel *)temp->model);
	temp->column = gtk_tree_view_column_new ();
	temp->cell_renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_set_title(temp->column, "Settings");
	gtk_tree_view_column_set_max_width(temp->column, 150);
	gtk_tree_view_column_pack_start(temp->column, temp->cell_renderer, TRUE);
	gtk_tree_view_column_set_attributes(temp->column,temp->cell_renderer,"text",0,NULL);
	gtk_tree_view_append_column((GtkTreeView *)temp->treeview, temp->column);
	gtk_container_add((GtkContainer *)temp->this,temp->treeview);
	gtk_tree_view_expand_all((GtkTreeView *)temp->treeview);
	return temp;
}

void gtk_settings_treeview_add(settings_treeview_t *setting_treeview, list_t *setting_list) {
	GtkTreeIter temp = {0,NULL,NULL,NULL};
	gtk_tree_store_append(setting_treeview->model, &temp,NULL);
	gtk_tree_store_set(setting_treeview->model,&temp,0,setting_list->name,1,setting_list, -1);
}

