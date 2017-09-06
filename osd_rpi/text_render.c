/*
Copyright (c) 2012, Broadcom Europe Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the copyright holder nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// Test app for VG font library.

#ifndef NO_OSD

#ifndef RESOURCE_DIR
#define RESOURCE_DIR "."
#endif
#define FONT_DIR RESOURCE_DIR"/osd_rpi"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#include "bcm_host.h"
#include "vgfont.h"

#include "../main_settings.h"
#include "../log.h"
#include "text_render.h"

#define TAG "text_render"

static int init = 0;
static int next_layer = 0;

static int32_t render_subtitle(text_render_t *tr)
{
   uint32_t width=0, height=0, x=0, y=0;
   int32_t s=0;
   uint32_t img_w, img_h;
   sem_wait(&tr->sem);
   char *textt = strdup(tr->text);
   sem_post(&tr->sem);
   uint32_t text_length = strlen(textt);

   graphics_get_resource_size(tr->img, &img_w, &img_h);

   if (text_length==0)
      return 0;

   s = graphics_resource_text_dimensions_ext(tr->img, textt, text_length, &width, &height, tr->text_size);
   if(width > tr->max_width) {
   		sem_wait(&tr->sem);
   		tr->text = strndup(textt,text_length - 3);
   		sem_post(&tr->sem);
   		s = render_subtitle(tr);
   } else {
       switch(tr->position) {
           case TR_POS_BOTTOM_LEFT:
               x = tr->x;
               y = tr->y + tr->max_height - height;
               break;
           case TR_POS_BOTTOM_RIGHT:
               x = tr->x + tr->max_width - width;
               y = tr->y + tr->max_height - height;
               break;
           default:
               x = tr->x + (tr->max_width - width) / 2;
               y = tr->y + tr->max_height - height;
       }
       s = graphics_resource_render_text_ext(tr->img, x, y,
                                     GRAPHICS_RESOURCE_WIDTH,
                                     GRAPHICS_RESOURCE_HEIGHT,
                                     GRAPHICS_RGBA32(0xff,0xff,0xff,tr->alpha), /* fg */
                                     GRAPHICS_RGBA32(0,0,0,tr->alpha < 0xa0 ? 0:0x80), /* bg */
                                     textt, text_length, tr->text_size);
   }
   return s;
}

text_render_t *tr_new() {
	int check = 0;
	text_render_t *temp = malloc(sizeof(text_render_t));

	if(!init) {
		bcm_host_init();
		init = 1;
	}

	check = gx_graphics_init(FONT_DIR);
	if(check) {
		LOGE(TAG, "gr_graphics_init failed! Is the font @ '%s'?", FONT_DIR);
		assert(0);
	}

	check = graphics_get_display_size(0, &temp->disp_width, &temp->disp_height);
	if(check) {
		LOGE(TAG, "%s", "graphics_get_display_size failed!");
		assert(0);
	}

	check = gx_create_window(0, temp->disp_width, temp->disp_height, GRAPHICS_RESOURCE_RGBA32, &temp->img);
	if(check) {
		LOGE(TAG, "%s", "gx_create_window failed!");
		assert(0);
	}

	graphics_resource_fill(temp->img, 0, 0, temp->disp_width, temp->disp_height, GRAPHICS_RGBA32(0,0,0,0x00));

	temp->x = 0;
	temp->y = 0;
	temp->text = "";
	temp->showing = 0;
	next_layer ++;
	temp->layer = next_layer;
	temp->alpha = 0xff;
	sem_init(&temp->sem,0,1);
	temp->max_width = 100;
	temp->max_height = 100;
	temp->text_size = 30;
	temp->thread = 0;
	temp->position = TR_POS_BOTTOM_CENTER;

	return temp;
}

void tr_set_xy(text_render_t *tr, uint32_t cx, uint32_t cy) {
	sem_wait(&tr->sem);
	tr->y = cy;
	tr->x = cx;
	sem_post(&tr->sem);
}

void tr_set_max_width(text_render_t *tr, uint32_t mw) {
	sem_wait(&tr->sem);
	tr->max_width = mw;
	sem_post(&tr->sem);
}

void tr_set_max_height(text_render_t *tr, uint32_t mh) {
	sem_wait(&tr->sem);
	tr->max_height = mh;
	sem_post(&tr->sem);
}

void tr_set_text_size(text_render_t *tr, uint32_t tz) {
	sem_wait(&tr->sem);
	tr->text_size = tz;
	sem_post(&tr->sem);
}

void tr_stop(text_render_t *tr) {
	sem_wait(&tr->sem); 
	tr->alpha = 1;
	sem_post(&tr->sem);
	pthread_join(tr->thread, NULL);
}

void tr_deinit() {
   init = 0;
   //graphics_delete_resource(img);
   //bcm_host_deinit();
}

void tr_set_text(text_render_t *tr, char * t) {
	sem_wait(&tr->sem);
	tr->text = strdup(t);
	sem_post(&tr->sem);
}

static void *tr_show(void * arg)
{
   text_render_t *tr = (text_render_t *)arg;
   if(!init) return NULL;
   tr->alpha = 0xff;

   graphics_resource_fill(tr->img, 0, 0, tr->disp_width, tr->disp_height, GRAPHICS_RGBA32(0,0,0,0x00));

   graphics_display_resource(tr->img, 0, tr->layer, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 1);

   while (tr->alpha > 0) {
      graphics_resource_fill(tr->img, 0, 0, tr->disp_width, tr->disp_height, GRAPHICS_RGBA32(0,0,0,0));
      render_subtitle(tr);
      graphics_update_displayed_resource(tr->img, 0, 0, 0, 0);
      tr->alpha -= 1;
   }
   graphics_display_resource(tr->img, 0, tr->layer, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 0);
   tr->showing = 0;
   return NULL;
}

void tr_show_thread(text_render_t *tr) {
	if(!tr->showing && osd_enable.int_value) {
		tr->showing = 1;
		pthread_create(&tr->thread, NULL, &tr_show, tr);
	} else
		tr->alpha = 0xff;
}

#endif