//
//  op_dbus.c
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

#include <dbus/dbus.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"
#include "op_dbus.h"
#include  <pthread.h>
#include "list.h"

#define TAG "op_dbus"

#define MPRIS_PATH "/org/mpris/MediaPlayer2"
#define MPRIS_NAME "org.mpris.MediaPlayer2.Player"
#define MPRIS_DEST "org.mpris.MediaPlayer2.omxplayer"

#define FDT_NAME "org.freedesktop.DBus.Properties"


static void export_session () {
	int fd;
    char file[255];
    char address[255];
	sprintf(file,"/tmp/omxplayerdbus.%s",getenv("USER"));
	fd = open(file, O_RDONLY);
	if (fd < 0) {
		LOGE(TAG, "Could not open session file %s\n",file);
		return;
	}
	if(!read(fd, address, 254)) {
		LOGE(TAG, "Session file %s was empty?\n",file);
		close(fd);
		return;
	}
	char * newline = index(address,'\n');
	newline[0] = '\0';
	//address[strlen(address) -1] = '\0';
	if(setenv("DBUS_SESSION_BUS_ADDRESS",address, 1)) {
		LOGE(TAG, "%s", "Could not set DBUS_SESSION_ADDRESS");
		close(fd);
		return;
	}
	close(fd);
}

static int create_connection(DBusConnection **connection) {
	DBusError error;
	dbus_error_init(&error);
	*connection = dbus_bus_get(DBUS_BUS_SESSION, &error);
	if(dbus_error_is_set (&error)) {
		LOGE(TAG, "%s\n",error.message);
		return 1;
	}
	return 0;
}

static int prep_message(DBusMessage **message, DBusMessageIter *iter, char *name, char *method) {
	char dest[255];
	*message = dbus_message_new_method_call(NULL, MPRIS_PATH, name, method);
	if(message == NULL) {
		LOGE(TAG,"%s","Could not create message");
		return 1;
	}
	sprintf(dest,"%s%d",MPRIS_DEST,getpid());
	dbus_message_set_auto_start(*message, TRUE);
	dbus_message_set_destination (*message, dest);
	dbus_message_iter_init_append (*message, iter);
	return 0;
}

static int send_message(char* name, char * method, list_t *args, void *reply_value) {
	DBusError error;
	DBusConnection *connection;
  	DBusMessage *message;
  	DBusMessageIter iter;
  	DBusMessageIter reply_iter;
  	export_session();
  	if(create_connection(&connection)) {
  		LOGE(TAG,"%s", "Failed to create connection");
  		return 1;
  	} 
#ifdef DEBUG_DBUS
	else
  		LOGD(TAG, "%s", "Connection Created");
#endif
  	if(prep_message(&message, &iter, name, method))
  		return 1;
#ifdef DEBUG_DBUS
  	else
  		LOGD(TAG, "%s", "Message Created");
#endif

  	for(int i = 0; i < args->count; i +=2) {
  		int *type = list_get_at_index(args,i);
  		void *value = list_get_at_index(args, i + 1);
  		dbus_message_iter_append_basic(&iter, *type, value);
  	}

  	DBusMessage *reply;
 	dbus_error_init (&error);
 	reply = dbus_connection_send_with_reply_and_block (connection,
                                                         message, 500,
                                                         &error);
#ifdef DEBUG_DBUS
    LOGD(TAG,"%s", "Message sent");
#endif
	if(dbus_error_is_set (&error)) {
		LOGE(TAG, "%s\n", error.message);
		goto end;
	}
	if(reply && reply_value) {
		dbus_message_iter_init(reply, &reply_iter);
		dbus_message_iter_get_basic(&reply_iter, reply_value);
	}
end:
	if(reply) 
  		dbus_message_unref (reply);
  	dbus_message_unref (message);
  	dbus_connection_unref (connection);
  	return 0;
}

void op_dbus_send_pause() {
	int value1 = 16;
	int type1 = DBUS_TYPE_INT32;
	list_t args = list_create("args",2, 1);
	list_add_entry(&args, &type1);
	list_add_entry(&args, &value1);
	send_message(MPRIS_NAME, "Action", &args, NULL);
	list_free(&args);
}

void op_dbus_send_stop() {
	int value1 = 15;
	int type1 = DBUS_TYPE_INT32;
	list_t args = list_create("args",2, 1);
	list_add_entry(&args, &type1);
	list_add_entry(&args, &value1);
	send_message(MPRIS_NAME, "Action", &args, NULL);
	list_free(&args);
}

void op_dbus_send_hidevideo() {
	int value1 = 28;
	int type1 = DBUS_TYPE_INT32;
	list_t args = list_create("args",2, 1);
	list_add_entry(&args, &type1);
	list_add_entry(&args, &value1);
	send_message(MPRIS_NAME, "Action", &args, NULL);
	list_free(&args);
}

void op_dbus_send_unhidevideo() {
	int value1 = 29;
	int type1 = DBUS_TYPE_INT32;
	list_t args = list_create("args",2, 1);
	list_add_entry(&args, &type1);
	list_add_entry(&args, &value1);
	send_message(MPRIS_NAME, "Action", &args, NULL);
	list_free(&args);
}

void op_dbus_send_setaspectmode(char *aspect) {
	char *value1 = "/not/used";
	int type1 = DBUS_TYPE_OBJECT_PATH;
	int type2 = DBUS_TYPE_STRING;
	list_t args = list_create("args",4, 1);
	list_add_entry(&args, &type1);
	list_add_entry(&args, &value1);
	list_add_entry(&args, &type2);
	list_add_entry(&args, &aspect);
	send_message(MPRIS_NAME, "SetAspectMode", &args, NULL);
	list_free(&args);
}

void op_dbus_send_setvideopos(int pos[]) {
	char buf[255];
	char *value1 = "/not/used";
	char *value2 = buf;
	int type1 = DBUS_TYPE_OBJECT_PATH;
	int type2 = DBUS_TYPE_STRING;
	sprintf(value2,"%d %d %d %d",pos[0] ,pos[1] ,pos[2], pos[3]);
	list_t args = list_create("args",4, 1);
	list_add_entry(&args, &type1);
	list_add_entry(&args, &value1);
	list_add_entry(&args, &type2);
	list_add_entry(&args, &value2);
	send_message(MPRIS_NAME, "VideoPos", &args, NULL);
	list_free(&args);
}

void op_dbus_send_setposition(long long microsec_position) {
	char *value1 = "/not/used";
	int type1 = DBUS_TYPE_OBJECT_PATH;
	int type2 = DBUS_TYPE_INT64;
	list_t args = list_create("args",4, 1);
	list_add_entry(&args, &type1);
	list_add_entry(&args, &value1);
	list_add_entry(&args, &type2);
	list_add_entry(&args, &microsec_position);
	send_message(MPRIS_NAME, "SetPosition", &args, NULL);
	list_free(&args);
}

long long op_dbus_send_duration() {
	long long ret = 0;
	char *value1 = MPRIS_NAME;
	int type1 = DBUS_TYPE_STRING;
	char *value2 = "Duration";
	int type2 = DBUS_TYPE_STRING;
	list_t args = list_create("args",4, 1);
	list_add_entry(&args, &type1);
	list_add_entry(&args, &value1);
	list_add_entry(&args, &type2);
	list_add_entry(&args, &value2);
	send_message(FDT_NAME, "Get", &args, &ret);
	list_free(&args);
	return ret;
}

long long op_dbus_send_position() {
	long long ret = 0;
	char *value1 = MPRIS_NAME;
	int type1 = DBUS_TYPE_STRING;
	char *value2 = "Position";
	int type2 = DBUS_TYPE_STRING;
	list_t args = list_create("args",4, 1);
	list_add_entry(&args, &type1);
	list_add_entry(&args, &value1);
	list_add_entry(&args, &type2);
	list_add_entry(&args, &value2);
	send_message(FDT_NAME, "Get", &args, &ret);
	list_free(&args);
	return ret;
}

int op_dbus_send_volume(double vol) {
	int ret = 0;
	char *value1 = MPRIS_NAME;
	int type1 = DBUS_TYPE_STRING;
	char *value2 = "Volume";
	int type2 = DBUS_TYPE_STRING;
	int type3 = DBUS_TYPE_DOUBLE;
	list_t args = list_create("args",6, 1);
	list_add_entry(&args, &type1);
	list_add_entry(&args, &value1);
	list_add_entry(&args, &type2);
	list_add_entry(&args, &value2);
	list_add_entry(&args, &type3);
	list_add_entry(&args, &vol);
	ret = send_message(FDT_NAME, "Set", &args, &ret);
	list_free(&args);
	return ret;
}
