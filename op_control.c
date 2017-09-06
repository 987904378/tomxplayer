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
#include <sys/types.h>
#include <sys/wait.h>

#define TAG "op_control"

static int pos[4];
static char *file_name;
static int is_running = 0;
static pthread_t thread;
static pthread_t out_read_thread;
static int hidden = 0;
static int alpha1 = 255;
static opc_playback_completed_func playback_completed_cb;
static void *playback_completed_user_data;

void opc_init() {
	/* Running this sets up the dbus session file in /tmp */
	system("omxplayer --version"
#ifndef DEBUG
		" > /dev/null"
#endif
	);
}
void opc_register_playback_completed(opc_playback_completed_func cb, void *user_data) {
	playback_completed_cb = cb;
	playback_completed_user_data = user_data;
}
void opc_stop_omxplayer() {
	if(is_running) {
		opc_playback_completed_func cb = playback_completed_cb;
		playback_completed_cb = NULL;
		op_dbus_send_stop();
		pthread_cancel(out_read_thread);
		pthread_join(out_read_thread, NULL);
		pthread_cancel(thread);
		pthread_join(thread, NULL);
		playback_completed_cb = cb;
		is_running = 0;
	}
}

static void *omxplayer_output_read(void *arg) {
	char *buf = NULL;
	size_t size = 0;
	FILE *fp = (FILE *)arg;
	while (getline(&buf, &size,fp) > 0) {
		buf[strlen(buf) -1] = '\0';
		LOGD(TAG, "%s", buf);
		if(strstr(buf, "have a nice day") != NULL) {
			LOGD(TAG, "%s", "End game found!");
			is_running = 0;
			int status = pclose(fp);
			if(playback_completed_cb != NULL) { playback_completed_cb(status, playback_completed_user_data); }
			break;
		}
	}
	return NULL;
}

static void *start_omxplayer_system(void *arg) {
	char *cmd;
	FILE *fp;
	is_running = 1;
	asprintf(&cmd, "omxplayer %s --alpha %d --dbus_name org.mpris.MediaPlayer2.omxplayer%d --adev %s --aspect-mode %s --no-keys --no-osd --win %d,%d,%d,%d '%s' 2>&1",
	omx_extra_args.string_value,
	hidden ? 0 : alpha1,
	getpid(),
	audio_out.string_value,
	stretch.int_value ? "stretch": "Letterbox", 
	pos[0] + arb_x_offset.int_value,
	pos[1] + arb_y_offset.int_value,
	pos[2] + arb_x_offset.int_value,
	pos[3] + arb_y_offset.int_value,
	file_name);
	LOGD(TAG, "starting omxplayer: %s",cmd);
	fp = popen(cmd, "r");
	free(cmd);
	if(fp < 0) {
		LOGE(TAG, "%s", "Trouble executing omxplayer!");
		is_running = 0;
		if(playback_completed_cb != NULL) { playback_completed_cb(-1, playback_completed_user_data); }
		return NULL;
	}
	pthread_create(&out_read_thread, NULL, &omxplayer_output_read, fp);
	pthread_join(out_read_thread, NULL);
	return NULL;
}

void opc_start_omxplayer_thread(int tpos[], char * tfile_name) {
	opc_stop_omxplayer();
	if(tpos[3] - tpos[1] <=0 || tpos[2] - tpos[0] <=0) {
		/* Can't have height or widget <= 0
		 * omxplayer uses the whole screen then
		 */
		LOGE(TAG, "%s","Calculated height or width <= 0!");
		return;
	}
	pos[0] = tpos[0];
	pos[1] = tpos[1];
	pos[2] = tpos[2];
	pos[3] = tpos[3];
	file_name = strdup(tfile_name);
	pthread_attr_t attr; 
	pthread_attr_init(&attr);
	pthread_create(&thread,&attr,&start_omxplayer_system,NULL);
}

void opc_update_pos(int tpos[]) {
	if(!is_running)return;
	tpos[0] += arb_x_offset.int_value;
	tpos[1] += arb_y_offset.int_value;
	tpos[2] += arb_x_offset.int_value;
	tpos[3] += arb_y_offset.int_value;
	op_dbus_send_setvideopos(tpos);
}

int opc_status(long long pbpos[]) {
	pbpos[0] = op_dbus_send_duration();
	pbpos[1] = op_dbus_send_position();
	return 0; 
}

void opc_toggle_playpause() {
	if(!is_running) return;
	op_dbus_send_pause();
}

int opc_set_volume(double vol) {
	if(!is_running) return 1;
	return op_dbus_send_volume(vol);
}

void opc_set_pb_position(unsigned long pos) {
	if(!is_running) return;
	op_dbus_send_setposition((long long)pos);
}

void opc_hidevideo() {
	hidden = 1;
	if(!is_running) return;
	op_dbus_send_hidevideo();
}

void opc_unhidevideo() {
	hidden = 0;
	if(!is_running) return;
	op_dbus_send_unhidevideo();
}

void opc_set_aspect(char * aspect) {
	if(!is_running) return;
	op_dbus_send_setaspectmode(aspect);
}

void opc_set_alpha(int alpha) {
	alpha1 = alpha;
	if(!is_running) return;
	op_dbus_send_setalpha((long long) alpha);
}

int opc_is_running() {
	return is_running;
}