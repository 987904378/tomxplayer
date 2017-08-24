//
//  list.h
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
#ifndef LIST_H
#define LIST_H

typedef struct {
	char *name;
	int count;
	int size;
	int step;
	int entrysize;
	void * list;
} list_t;

list_t list_create(char *name, int initial_count,int step);
void list_add_entry(list_t *list, void *entry);
void *list_get_at_index(list_t *list, int index);
void list_free(list_t *list);

#endif
