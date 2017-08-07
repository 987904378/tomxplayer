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

typedef void (* opc_playback_completed_func) (int exit_code, void *user_data);

void opc_register_playback_completed(opc_playback_completed_func cb, void *user_data);
void opc_start_omxplayer_thread(int pos[], char * file_name);
void opc_update_pos(int pos[]);
void opc_set_pb_position(unsigned long pos);
int opc_status(long long pbpos[]);
void opc_hidevideo();
void opc_unhidevideo();
void opc_toggle_playpause();
void opc_stop_omxplayer();
int opc_set_volume(double vol);
void opc_set_aspect(char *aspect);
void opc_set_alpha(int alpha);
int opc_is_running();

#endif