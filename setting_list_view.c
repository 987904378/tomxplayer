//
//  settings_list_view.c
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
#include "setting_list_view.h"
#include "settings.h"
#include "log.h"
#include "list.h"

#ifdef GTK3
#include "gtk3_compat.h"
#endif

#define MARGIN 3

#define TAG "setting_list_view"

static void setting_vbox_size_allocated(GtkWidget *widget, GtkAllocation *allocation, gpointer user_data) {
	GtkWidget *label_desc = (GtkWidget *)user_data;
	gtk_widget_set_size_request((GtkWidget *)label_desc,allocation->width - (MARGIN * 8), -1);
}
static void build_setting_viewer_header(GtkVBox *vbox, setting_t *setting) {
	GtkWidget * label_title = gtk_label_new(setting->long_name);
#ifdef GTK3
	gtk_label_set_xalign((GtkLabel *)label_title,0);
	gtk_label_set_yalign((GtkLabel *)label_title,0);
#else
	gtk_misc_set_alignment((GtkMisc *)label_title,0,0);
#endif
	gtk_box_pack_start((GtkBox *)vbox, label_title, FALSE, FALSE, MARGIN);
}
static void build_setting_viewer_footer(GtkVBox *vbox, setting_t *setting) {
	GtkWidget *label_desc = gtk_label_new(setting->desc);
	g_signal_connect((GObject *)vbox,"size-allocate",G_CALLBACK(setting_vbox_size_allocated), label_desc);
	gtk_label_set_line_wrap ((GtkLabel *)label_desc, TRUE);
	gtk_widget_set_size_request((GtkWidget *)label_desc,350, 0);
#ifdef GTK3
	gtk_label_set_xalign((GtkLabel *)label_desc,0);
	gtk_label_set_yalign((GtkLabel *)label_desc,0);
#else
	gtk_misc_set_alignment((GtkMisc *)label_desc,0,0);
#endif
	gtk_widget_set_sensitive((GtkWidget *)label_desc, FALSE);
	gtk_box_pack_start((GtkBox *)vbox, label_desc, FALSE, FALSE, MARGIN * 2);
	gtk_box_pack_end((GtkBox *)vbox, gtk_hseparator_new(), FALSE, FALSE, MARGIN * 2);
}

static void entry_changed(GtkEditable *editable, gpointer user_data) {
	GtkEntry *entry = (GtkEntry *)editable;
	setting_t *setting = (setting_t *)user_data;
	setting->string_value = strdup(gtk_entry_get_text(entry));
	settings_save(setting);
}

static GtkWidget *gtk_string_setting_viewer_new(setting_t *setting) {
	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
	build_setting_viewer_header((GtkVBox *)vbox, setting);
	GtkWidget *combo_or_entry;
	if(!setting->option_count) {
		combo_or_entry = gtk_entry_new();
		gtk_entry_set_text((GtkEntry *)combo_or_entry,setting->string_value);
		g_signal_connect((GObject *)combo_or_entry,"changed",G_CALLBACK(entry_changed), setting);
	} else {
#ifdef GTK3	
		combo_or_entry = gtk_combo_box_text_new_with_entry();
		char *option = setting->option_array;
		for(int i =0;i < setting->option_count;i ++) {
			gtk_combo_box_text_append((GtkComboBoxText *)combo_or_entry, NULL, option);
			void *offset = (void *)option + strlen(option) + 1;
			option = (char *)offset;
		}

		GtkEntry *entry = (GtkEntry *)gtk_bin_get_child ((GtkBin * )combo_or_entry);
#else
		GtkTreeStore *model = gtk_tree_store_new(1, G_TYPE_STRING);
		char *option = setting->option_array;
		for(int i =0;i < setting->option_count;i ++) {
			GtkTreeIter iter = {0, NULL, NULL, NULL};
			gtk_tree_store_append(model, &iter, NULL);
			gtk_tree_store_set(model, &iter ,0 , option, -1);
			void *offset = (void *)option + strlen(option) + 1;
			option = (char *)offset;
		}

		combo_or_entry = gtk_combo_box_entry_new_with_model((GtkTreeModel *)model, 0);
		GtkEntry *entry = GTK_ENTRY (GTK_BIN (combo_or_entry)->child);
#endif
		gtk_entry_set_text(entry,setting->string_value);
		g_signal_connect((GObject *)entry,"changed",G_CALLBACK(entry_changed), setting);
	}
	gtk_box_pack_start((GtkBox *)vbox, combo_or_entry, FALSE, FALSE, MARGIN);
	build_setting_viewer_footer((GtkVBox *)vbox, setting);
	gtk_widget_show_all(vbox);
	return vbox;
}

static void checkbutton_toggled(GtkToggleButton *togglebutton, gpointer user_data){
	gboolean newvalue = gtk_toggle_button_get_active(togglebutton);
	setting_t *setting = (setting_t *)user_data;
	setting->int_value = (int)newvalue;
	settings_save(setting);
}

static GtkWidget *gtk_bool_setting_viewer_new(setting_t *setting) {
	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
	GtkWidget *checkbutton = gtk_check_button_new_with_label (setting->long_name);
	gtk_toggle_button_set_active((GtkToggleButton *)checkbutton, (gboolean)setting->int_value);
	g_signal_connect((GObject *)checkbutton,"toggled",G_CALLBACK(checkbutton_toggled), setting);
	gtk_box_pack_start((GtkBox *)vbox, checkbutton, FALSE, FALSE, MARGIN);
	build_setting_viewer_footer((GtkVBox *)vbox, setting);
	gtk_widget_show_all(vbox);
	return vbox;
}

static void spinbutton_value_changed(GtkSpinButton *spinbutton, gpointer user_data) {
	gdouble newvalue = gtk_spin_button_get_value(spinbutton);
	setting_t *setting = (setting_t *)user_data;
	setting->int_value = (int)newvalue;
	settings_save(setting);
}

static GtkWidget *gtk_int_setting_viewer_new(setting_t *setting) {
	GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
	build_setting_viewer_header((GtkVBox *)vbox, setting);
	GtkWidget *spinbutton = gtk_spin_button_new_with_range((gdouble)setting->min,(gdouble)setting->max, 1);
	g_signal_connect((GObject *)spinbutton,"value-changed",G_CALLBACK(spinbutton_value_changed), setting);
	gtk_spin_button_set_value((GtkSpinButton *)spinbutton, setting->int_value);
	gtk_box_pack_start((GtkBox *)vbox, spinbutton, FALSE, FALSE, MARGIN);
	build_setting_viewer_footer((GtkVBox *)vbox, setting);
	gtk_widget_show_all(vbox);
	return vbox;
}

setting_list_view_t *gtk_setting_list_view_new() {
	setting_list_view_t *temp = malloc(sizeof(setting_list_view_t));
	temp->this = gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy((GtkScrolledWindow *)temp->this,GTK_POLICY_NEVER,GTK_POLICY_AUTOMATIC);
	temp->vbox = gtk_vbox_new(FALSE, 0);
#ifdef GTK3
	gtk_container_add((GtkContainer *)temp->this, temp->vbox);
#else
	gtk_scrolled_window_add_with_viewport((GtkScrolledWindow *)temp->this, temp->vbox);
#endif
	gtk_container_set_border_width((GtkContainer *)temp->vbox, MARGIN * 4);
	temp->vbox_children = list_create("vbox_children",1,1);
	return temp;
}

void gtk_setting_list_view_free(setting_list_view_t *slv) {
	list_free(&slv->vbox_children);
	free(slv);
}

void gtk_setting_list_view_add(setting_list_view_t *slv, list_t *settings) {
	for(int i = 0;i < slv->vbox_children.count; i++) {
		GtkWidget *widget = (GtkWidget *) list_get_at_index(&slv->vbox_children,i);
		gtk_widget_destroy(widget);
	}
	list_free(&slv->vbox_children);
	slv->vbox_children = list_create("vbox_children",1,1);
	LOGD(TAG,"Adding settings list with count of %d",settings->count);
	for(int i = 0; i < settings->count; i ++) {
		setting_t *setting = list_get_at_index(settings, i);
		if(!setting->user_editable) continue;
		LOGD(TAG, "Setting %s; type=%d",setting->name, setting->type);
		if(setting->type == SETTINGS_TYPE_INT) {
			LOGD(TAG, "Setting %s is typeof INT", setting->name);
			GtkWidget *isv = gtk_int_setting_viewer_new(setting);
			list_add_entry(&slv->vbox_children, isv);
		gtk_box_pack_start((GtkBox *)slv->vbox,isv,FALSE,FALSE,10);
		} else if(setting->type == SETTINGS_TYPE_BOOL) {
			LOGD(TAG, "Setting %s is typeof BOOL", setting->name);
			GtkWidget *bsv = gtk_bool_setting_viewer_new(setting);
			list_add_entry(&slv->vbox_children, bsv);
		gtk_box_pack_start((GtkBox *)slv->vbox,bsv,FALSE,FALSE,10);
		} else if(setting->type == SETTINGS_TYPE_STRING) {
			LOGD(TAG, "Setting %s is typeof STRING", setting->name);
			GtkWidget *ssv = gtk_string_setting_viewer_new(setting);
			list_add_entry(&slv->vbox_children, ssv);
			gtk_box_pack_start((GtkBox *)slv->vbox,ssv,FALSE,FALSE,10);
		}
	}
	gtk_widget_show_all(slv->vbox);
}
