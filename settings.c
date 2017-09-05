//
//  settings.c
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
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include "log.h"
#include "settings.h"

#define TAG "settings"

GKeyFile *key_file;
GError *error;
gsize length;
char file[255];
char dir[255];

void settings_init() {
	char * home = getenv("HOME");
	sprintf(dir,"%s/.tomxplayer", home);
	mkdir(dir, 00775);
	sprintf(file,"%s/tomxplayer.ini", dir);
	LOGI(TAG, "Using settings file @ %s",file);
	FILE * temp = fopen(file, "a");
	fclose(temp);
	key_file = g_key_file_new();
	error = NULL;
	if(!g_key_file_load_from_file(key_file, file, G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, &error)) {
		LOGE(TAG,"%s", "Could not load settings file...");
		if(error != NULL)
			LOGE(TAG, "%s", error->message);
	}
}

void settings_save(setting_t *setting) {
	error = NULL;
	if(setting->type) {
		g_key_file_set_integer(key_file, setting->catagory, setting->name, setting->int_value);
		LOGI(TAG, "Updated setting '%s' -> '%d'",setting->name, setting->int_value);
	} else {
		g_key_file_set_string(key_file, setting->catagory, setting->name, setting->string_value);
		LOGI(TAG, "Updated setting '%s' -> '%s'",setting->name, setting->string_value);
	}
	if(!g_key_file_save_to_file(key_file, file, &error)) {
		LOGE(TAG, "%s", "Could not save settings to file...");
		if(error != NULL)
			LOGE(TAG,"%s",error->message);
	} else {
		LOGI(TAG,"%s", "Settings saved.");
		if(setting->setting_update_cb != NULL) { setting->setting_update_cb(setting, setting->setting_update_cb_user_data); }
	} 
}

void settings_read(setting_t *setting) {
	error = NULL;
	if(setting->type) {
		int ret = g_key_file_get_integer(key_file, setting->catagory, setting->name, &error);
		if(error == NULL) {
			if(ret != setting->int_value){
				setting->int_value = ret;
				if(setting->setting_update_cb != NULL) { setting->setting_update_cb(setting, setting->setting_update_cb_user_data); }
			}
		}
	} else {
		char * ret = g_key_file_get_string(key_file, setting->catagory, setting->name, &error);
		if(error == NULL) {
			if(strcmp(ret,setting->string_value)){
				setting->string_value = ret;
				if(setting->setting_update_cb != NULL) { setting->setting_update_cb(setting, setting->setting_update_cb_user_data); }
			}
		}
	}
	if(error != NULL) {
		LOGE(TAG, "%s",error->message); 
	}
}

