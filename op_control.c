//
//  op_control.c
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

#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include "op_control.h"
#include "log.h"
#include "main_settings.h"
#include "op_dbus.h"

#define TAG "op_control"

int pos[4];
char *file_name;
int is_running = 0;
pthread_t update_thread;

void stop_omxplayer() {
	system("./dbuscontrol.sh stop 2>&1 > /dev/null");
	system("killall omxplayer 2>&1 > /dev/null");
	system("killall omxplayer.bin 2>&1 > /dev/null");
}

void *start_omxplayer_system(void *arg) {
	char cmd[255];
	stop_omxplayer(); 
	is_running = 1;
	sprintf(cmd, "omxplayer --adev %s --aspect-mode %s --no-keys --no-osd --win %d,%d,%d,%d '%s' 2>&1 > /dev/null", 
	audio_out.string_value,
	stretch.int_value ? "stretch": "Letterbox", 
	pos[0],pos[1],pos[2],pos[3], file_name);
	system(cmd);
	is_running = 0;
}

void start_omxplayer_thread(int tpos[], char * tfile_name) {
	pos[0] = tpos[0];
	pos[1] = tpos[1];
	pos[2] = tpos[2];
	pos[3] = tpos[3];
	file_name = strdup(tfile_name);
	pthread_t thread;
	pthread_attr_t attr; 
	pthread_attr_init(&attr);
	pthread_create(&thread,&attr,&start_omxplayer_system,NULL);
}

void * update_pos(void *arg) {
	pthread_detach(pthread_self());
	if(!is_running)return NULL;
	int *tpos = (int *)arg;
	//char cmd[255];  
	//sprintf(cmd, "./dbuscontrol.sh setvideopos %d %d %d %d 2>&1 > /dev/null", tpos[0], tpos[1], tpos[2], tpos[3]);
	//system(cmd);
	op_dbus_send_setvideopos(tpos);
}

void update_pos_thread(int tpos[]) {
	//pthread_cancel(update_thread);
	if(!is_running)return;
	//pthread_create(&update_thread, NULL, &update_pos, tpos);
	op_dbus_send_setvideopos(tpos);
}

int status(long long pbpos[]) {
	pbpos[0] = op_dbus_send_duration();
	pbpos[1] = op_dbus_send_position();
//	FILE * fp = popen("./dbuscontrol.sh status 2> /dev/null","r");
//	size_t size = 0;
//	char *buf = NULL;
//	if(fp == NULL) {
//		LOGD(TAG, "%s", "Could not execute 'dbuscontrol.sh status'\n");
//		return 1;
//	}
//	if(getline(&buf, &size,fp) < 0) {
//		LOGD(TAG, "%s", "Could not read result of 'dbuscontrol.sh status'\n");
//		return 1;
//	}
//
//	char * dur = index(buf, ' ');
//	dur += 1;
//	dur[strlen(dur) -1] = '\0';
//	pbpos[0] = atoll(dur);
//
//	getline(&buf, &size,fp);
//	char * pos = index(buf,' ');
//	pos += 1;
//	pos[strlen(pos) -1] = '\0';
//	pbpos[1] = atoll(pos);
//	if(pbpos[1] < 0) pbpos[1] = 0;
//	/* Throw away: if we do not read the last line
//	* the we get a broken pipe error. However
//	* we are not using it.
//	*/
//	getline(&buf, &size,fp);
//	pclose(fp);
	return 0; 
}

void toggle_playpause() {
	if(!is_running) return;
	//system("./dbuscontrol.sh pause");
	op_dbus_send_pause();
	op_dbus_send_duration();
}

int set_volume(double vol) {
	if(!is_running) return 1;
	//char cmd[255]; 
	//sprintf(cmd, "./dbuscontrol.sh volume %1.2F 2>&1 > /dev/null", vol); 
	//return system(cmd);
	return op_dbus_send_volume(vol);
}

void set_pb_position(unsigned long pos) {
	if(!is_running) return;
	//char cmd[255]; 
	//sprintf(cmd, "./dbuscontrol.sh setposition %lu 2>&1 > /dev/null", pos);
	//system(cmd);
	op_dbus_send_setposition((long long)pos);
}

void hidevideo() {
	if(!is_running) return;
	//system("./dbuscontrol.sh hidevideo");
	op_dbus_send_hidevideo();
}

void unhidevideo() {
	if(!is_running) return;
	//system("./dbuscontrol.sh unhidevideo");
	op_dbus_send_unhidevideo();
}

void set_aspect(char * aspect) {
	if(!is_running) return;
	//char cmd[255]; 
	//sprintf(cmd, "./dbuscontrol.sh setaspectmode %s 2>&1 > /dev/null", aspect);
	//system(cmd);
	op_dbus_send_setaspectmode(aspect);
}

int op_is_running() {
	return is_running;
}
