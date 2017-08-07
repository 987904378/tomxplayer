//
//  op_dbus.h
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

void op_dbus_send_pause();
void op_dbus_send_stop();
void op_dbus_send_hidevideo();
void op_dbus_send_unhidevideo();
void op_dbus_send_setaspectmode(char *aspect);
void op_dbus_send_setvideopos(int pos[]);
void op_dbus_send_setposition(int64_t *ms);
void op_dbus_send_setalpha(int alpha);
int64_t op_dbus_send_duration();
int64_t op_dbus_send_position();
int op_dbus_send_volume(double vol);