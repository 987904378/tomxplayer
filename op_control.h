//
//  op_control.h
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

#ifndef OP_CONTROL_H
#define OP_CONTROL_H

void start_omxplayer_thread(int pos[], char * file_name);
void update_pos_thread(int pos[]);
void set_pb_position(unsigned long pos);
int status(long long pbpos[]);
void hidevideo();
void unhidevideo();
void toggle_playpause();
void stop_omxplayer();
int set_volume(double vol);
int ms_to_time(unsigned long ms, char * dest);
void set_aspect(char *aspect);
void set_aout(char * aout);
int op_is_running();

#endif
