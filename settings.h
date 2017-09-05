//
//  setting.h
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

#ifndef SETTINGS_H
#define SETTINGS_H

enum SETTINGS_TYPE {
  SETTINGS_TYPE_STRING = 0,
  SETTINGS_TYPE_INT,
  SETTINGS_TYPE_BOOL,
};

typedef struct {
	char *name;
	char *long_name;
	char *catagory;
	char *path;
	char *string_value;
	int option_count;
	int int_value;
	int min;
	int max;
	int type;
	int user_editable;
	char *desc;
	void (* setting_update_cb)(void *setting, void* user_data);
	void *setting_update_cb_user_data;
	char *option_array;
} setting_t;

void settings_init();
void settings_read(setting_t *setting);
void settings_save(setting_t *setting);

#endif
