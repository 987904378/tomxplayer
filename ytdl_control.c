//
//  ytdl_control.c
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

#include <stdio.h>
#include "log.h"
#include "ytdl_control.h"
#include <string.h>
#include <pthread.h>

#define TAG "ytdl_control"

static ytdl_output_cb output_cb;
static ytdl_output_cb url_cb;
static pthread_t thread;

void ytdl_register_output_cb(ytdl_output_cb cb) {
	output_cb = cb;
}
void ytdl_register_url_cb(ytdl_output_cb cb) {
	url_cb = cb;
}
void *ytdl_cget_url(void *arg) {
	char *urlin =  (char *)arg;
	char cmd[1024];
	sprintf(cmd, "youtube-dl -f '(mp4)[height<480]' --get-url '%s' 2>&1", urlin);
	FILE * fp = popen(cmd,"r");
	size_t size = 0;
	char *buf = NULL;
	if(fp == NULL) {
	     LOGE(TAG, "Could not execute '%s'", cmd);
	     return NULL;
	}
	while (getline(&buf, &size,fp) > -1) {
		LOGD(TAG, "%s", buf);
		if(strncmp(buf, "http", 4))
			if(output_cb != NULL) { output_cb(buf);}
	}
	pclose(fp);
	if(!strncmp(buf, "http", 4)) {
		LOGD(TAG, "URL=%s",buf);
		if(url_cb != NULL) { url_cb(buf);}
	} else {
		LOGE(TAG, "%s", "No valid URL found.");
	}
	return NULL;
}
void ytdl_cget_url_thread(char *url) {
	pthread_create(&thread,NULL, ytdl_cget_url, strdup(url));
}
#if 0

int main(int argc, char *argv[]) {
	char new_url[1024];
	if(argc > 1) {
		ytdl_cget_url(argv[1], new_url);
		printf("%s",new_url);
	}
	return 0;
}

#endif