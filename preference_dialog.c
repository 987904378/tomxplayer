//
//  preference_dialog.c
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
#include <string.h>
#include "settings.h"
#include "setting_treeview.h"
#include "preference_dialog.h"
#include "list.h"
#include "log.h"
#include "setting_list_view.h"

#ifdef GTK3
#include "gtk3_compat.h"
#endif

#define TAG "preference_dialog"

static void treeview_cursor_changed(GtkTreeView *treeview, gpointer user_data) {
	LOGD(TAG,"%s","cursor_changed");
	GtkTreeModel *m;
	GtkTreeIter i = {0,NULL,NULL,NULL};
	GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
	gtk_tree_selection_get_selected (selection, &m, &i);
	if(!i.stamp) return;
	char *context;
	list_t *settings_list;
	gtk_tree_model_get(m,&i,0,&context,1,&settings_list,-1);
	LOGD(TAG,"Context is now %s",context);
	preference_dialog_t *temp = ( preference_dialog_t *)user_data;
	gtk_setting_list_view_add(temp->setting_list_view, settings_list);
}

static void preference_dialog_on_show(GtkWidget * widget, gpointer user_data) {
#ifndef GTK3
	gtk_window_set_policy((GtkWindow *)widget,FALSE,TRUE,FALSE);
#endif
}

preference_dialog_t *gtk_preference_dialog_new(GtkWindow * parent) {
	preference_dialog_t *temp = malloc(sizeof(preference_dialog_t));
	temp->window = gtk_dialog_new_with_buttons(
								"Preferences",
								parent, 
								GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
#ifdef GTK3
								"_OK",
#else
								GTK_STOCK_OK,
#endif
								GTK_RESPONSE_ACCEPT,
								NULL);
	gtk_window_set_default_size((GtkWindow *)temp->window, 640, 480);
	gtk_widget_set_events((GtkWidget *)temp->window, GDK_ALL_EVENTS_MASK);
	g_signal_connect((GtkObject *)temp->window,"show",G_CALLBACK(preference_dialog_on_show), NULL);
	GtkWidget *hbox = gtk_hbox_new(FALSE,6);
#ifdef GTK3
	gtk_box_pack_start((GtkBox *)gtk_dialog_get_content_area ((GtkDialog *) temp->window),hbox,TRUE,TRUE,0);
#else
	gtk_container_add((GtkContainer *)((GtkDialog *)temp->window)->vbox, hbox);
#endif 
	temp->settings_treeview = gtk_settings_treeview_new();
	g_signal_connect((GtkObject *)temp->settings_treeview->treeview,"cursor-changed",G_CALLBACK(treeview_cursor_changed), temp);
	gtk_widget_set_size_request(temp->settings_treeview->this, 200, -1);
	gtk_box_pack_start((GtkBox *)hbox, temp->settings_treeview->this, FALSE, FALSE, 0);
	temp->setting_list_view = gtk_setting_list_view_new();
	gtk_box_pack_start((GtkBox *)hbox, temp->setting_list_view->this, TRUE, TRUE, 0);
	gtk_widget_show_all(hbox);
	return temp;
}
void gtk_preference_dialog_free(preference_dialog_t *pd) {
	gtk_setting_list_view_free(pd->setting_list_view);
	free(pd);
}
void gtk_preference_dialog_add(preference_dialog_t *pd, list_t *setting_list) {
	gtk_settings_treeview_add(pd->settings_treeview, setting_list);
}
