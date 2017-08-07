//
//  media_playlist.h
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
#ifndef MEDIA_PLAYLIST_H
#define MEDIA_PLAYLIST_H

#include "list.h"

typedef struct {
	int index;
	list_t list;
} media_playlist_t;

int is_media_by_ext(const char *path);
media_playlist_t *mp_create();
media_playlist_t *mp_create_dir_of_file(char * path);
void mp_add(media_playlist_t *plist, char *full_path);
char* mp_get_current(media_playlist_t *plist);
void mp_move_next(media_playlist_t *plist);
void mp_move_previous(media_playlist_t *plist);
void mp_free(media_playlist_t *plist);

#endif