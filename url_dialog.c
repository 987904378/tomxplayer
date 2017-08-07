//
//  url_dialog.c
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

#include "url_dialog.h"

static void entry_changed(GtkEditable *editable, gpointer user_data) {
	GtkEntry *entry = (GtkEntry *)editable;
	url_dialog_t *temp = (url_dialog_t *)user_data;
	temp->url = strdup(gtk_entry_get_text(entry));
}

url_dialog_t *gtk_url_dialog_new(GtkWindow * parent) {
	url_dialog_t *temp = malloc(sizeof(url_dialog_t));
	temp->window = gtk_dialog_new_with_buttons(
								"Open a URL",
								parent, 
								GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
#ifdef GTK3
								"_Cancel",
#else
								GTK_STOCK_CANCEL,
#endif
								GTK_RESPONSE_CANCEL,
#ifdef GTK3
								"_OK",
#else
								GTK_STOCK_OK,
#endif
								GTK_RESPONSE_ACCEPT,
								"_Use 'youtube-dl'",
								GTK_RESPONSE_APPLY,

								NULL);
	gtk_window_set_default_size((GtkWindow *)temp->window, 640, 200);
	GtkWidget *entry = gtk_entry_new();
	g_signal_connect((GObject *)entry,"changed",G_CALLBACK(entry_changed), temp);
	GtkWidget *window_box;
#ifdef GTK3
	window_box = gtk_dialog_get_content_area((GtkDialog *) temp->window);
	gtk_box_pack_start((GtkBox *)window_box ,entry,TRUE,TRUE,0);
#else
	window_box = ((GtkDialog *)temp->window)->vbox;
	gtk_container_add((GtkContainer *)window_box, entry);
#endif
	gtk_widget_show(entry);
	return temp;
}