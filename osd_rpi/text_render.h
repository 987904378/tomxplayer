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
#include <unistd.h>
#include <pthread.h>
#include "vgfont.h"
#include "bcm_host.h"

typedef enum TR_DISPLAY_POSITION {
  TR_POS_BOTTOM_LEFT = 0,
  TR_POS_BOTTOM_CENTER,
  TR_POS_BOTTOM_RIGHT,
} tr_display_pos_t;

typedef struct {
	GRAPHICS_RESOURCE_HANDLE img;
	uint32_t x;
	uint32_t y;
	uint32_t max_width;
	uint32_t max_height;
	uint32_t disp_height;
	uint32_t disp_width;
	char *text;
	int showing;
	int layer;
	uint32_t alpha;
	pthread_mutex_t mutex;
	uint32_t text_size;
	pthread_t thread;
	tr_display_pos_t position;
} text_render_t;

text_render_t *tr_new();
void tr_set_text(text_render_t *tr, char * t);
void tr_set_xy(text_render_t *tr, uint32_t cx, uint32_t cy);
void tr_set_max_width(text_render_t *tr, uint32_t mw);
void tr_set_max_height(text_render_t *tr, uint32_t mh);
void tr_set_text_size(text_render_t *tr, uint32_t tz);
void tr_stop(text_render_t *tr);
void tr_deinit();
void tr_show_thread(text_render_t *tr);

#endif