//
//  text_render.h
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

#ifndef TEXT_RENDER_H
#define TEXT_RENDER_H
#include <stdint.h>

void tr_init();
void tr_set_xy(uint32_t cx, uint32_t cy);
void tr_set_max_width(uint32_t mw);
void tr_set_text_size(uint32_t tz);
void tr_stop();
void tr_deinit();
void tr_show_thread(char * string);

#endif