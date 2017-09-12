//
//  media_playlist.c
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

#include <libgen.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "settings.h"
#include "list.h"
#include "main_settings.h"
#include <errno.h>

#include "media_playlist.h"

#define TAG "media_playlist"

int is_media_by_ext(const char *path) {
	char *spaced;
	char *extfound;
	char *ext = rindex(path, '.');
	if(ext == NULL)
		return 0;
	LOGD(TAG,"'%s' ext=%s",path,ext);
	if(strlen(ext) > 1)
		ext += 1;
	else
		return 0;
	asprintf(&spaced,"%s ", ext);
	extfound = strstr(file_types.string_value, spaced);
	if(extfound == NULL)
		return 0;
	return 1;
}

static int scandir_filter(const struct dirent *dirent) {
	return is_media_by_ext(dirent->d_name);
}

media_playlist_t *mp_create() {
	media_playlist_t *plist = malloc(sizeof(media_playlist_t));
	plist->list = list_create("playlist",10,10);
	plist->index = 0;
	return plist;
};

media_playlist_t *mp_create_dir_of_file(char * path) {
	char *full_path;
	char *dir_path = dirname(strdup(path));
	struct dirent **files = NULL;
	media_playlist_t *plist = mp_create();
	int file_count = scandir(dir_path, &files, scandir_filter, alphasort);
	if(file_count < 1) {
		LOGE(TAG, "No media files found for dir in %s",dir_path);
		if(file_count < 0)
			LOGE(TAG, "%s", strerror(errno));
		return NULL;
	}
	for(int i = 0; i < file_count; i ++) {
		asprintf(&full_path, "%s/%s",dir_path, files[i]->d_name);
		free(files[i]);
		if(!strcmp(path,full_path))
			plist->index = plist->list.count;
		list_add_entry(&plist->list, strdup(full_path));
		LOGI(TAG,"Added %s to playlist.",full_path);
		free(full_path);
	}
	free(dir_path);
	return plist;
}

void mp_add(media_playlist_t *plist, char *full_path) {
	list_add_entry(&plist->list, strdup(full_path));
}

char *mp_get_current(media_playlist_t *plist) {
	char *temp = (char *)list_get_at_index(&plist->list, plist->index);
	LOGD(TAG,"getcurrent=%s",temp);
	return temp;
}

void mp_move_next(media_playlist_t *plist) {
	if(plist->index + 1 >= plist->list.count)
		plist->index = 0;
	else
		plist->index ++;
}

void mp_move_previous(media_playlist_t *plist) {
	if(plist->index -1 < 0)
		plist->index = plist->list.count -1;
	else
		plist->index --;
}

void mp_free(media_playlist_t *plist) {
	for(int i = 0;i < plist->list.count;i ++) {
		free(list_get_at_index(&plist->list, i));
	}
	list_free(&plist->list);
	free(plist);
}
