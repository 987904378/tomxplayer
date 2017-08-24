//
//  list.c
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

#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "log.h"

#define TAG "list"

list_t list_create(char *name,int initial_count,int step) {
	void * buf = malloc(initial_count * sizeof(void *));
	list_t temp = {
		name,
		0,
		initial_count,
		step,
		sizeof(void *),
		buf,
	};
#ifdef DEBUG_LIST
	LOGD(TAG, "Create list %s initial_count =%d", name, initial_count);
#endif
	return temp;
}

void list_add_entry(list_t *list, void *entry) {
	if(list->count == list->size) {
		list->size += list->step;
		list->list = realloc(list->list, list->size * list->entrysize);
#ifdef DEBUG_LIST
		LOGD(TAG,"list %s resized: count=%d, size=%d",list->name,list->count, list->size);
#endif
	}
	void *offset = list->list + (list->count * list->entrysize);
	memcpy(offset, &entry,list->entrysize);
	list->count ++;
}

void *list_get_at_index(list_t *list, int index) {
	void *offset = list->list + (index * list->entrysize);
	return *(void **)offset;
}

void list_free(list_t *list) {
	free(list->list);
}
