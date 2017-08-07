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

static GRAPHICS_RESOURCE_HANDLE img;
static uint32_t x,y = 0;
static char text[255];
static int showing = 0;
static uint32_t alpha = 0xff;
static int dinit = 0;
static sem_t sem;
static uint32_t max_width = 100;
static uint32_t text_size = 30;
static pthread_t thread;

static int32_t render_subtitle(GRAPHICS_RESOURCE_HANDLE img, const uint32_t text_size, const char * textt)
{
   uint32_t text_length = strlen(textt);
   uint32_t width=0, height=0;
   int32_t s=0;
   uint32_t img_w, img_h;

   graphics_get_resource_size(img, &img_w, &img_h);

   if (text_length==0)
      return 0;

   s = graphics_resource_text_dimensions_ext(img, textt, text_length, &width, &height, text_size);
   if(width > max_width) {
   		s = render_subtitle(img,text_size,strndup(textt,text_length - 3));
   } else {
   s = graphics_resource_render_text_ext(img, x + (max_width - width) / 2, y - height,
                                     GRAPHICS_RESOURCE_WIDTH,
                                     GRAPHICS_RESOURCE_HEIGHT,
                                     GRAPHICS_RGBA32(0xff,0xff,0xff,alpha), /* fg */
                                     GRAPHICS_RGBA32(0,0,0,alpha < 0xa0 ? 0:0x80), /* bg */
                                     textt, text_length, text_size);
   }
   return s;
}

void tr_init() {
   int s;

   bcm_host_init();

   s = gx_graphics_init(FONT_DIR);
   assert(s == 0);
   sem_init(&sem,0,1);
}

void tr_set_xy(uint32_t cx, uint32_t cy) {
	y = cy;
	x = cx;
}

void tr_set_max_width(uint32_t mw) {
	max_width = mw;
}

void tr_set_text_size(uint32_t tz) {
	text_size = tz;
}

void tr_stop() {
	alpha = 1;
}

void tr_deinit() {
   dinit = 1;
   tr_stop();
   graphics_delete_resource(img);
   bcm_host_deinit();
}

static void tr_set_text(char * t) {
	sem_wait(&sem);
	sprintf(text,"%s",t);
	free(t);
	sem_post(&sem);
}

static void *tr_show(void * string_arg)
{
   if(!osd_enable.int_value)
		return NULL;
   char * string = (char *)string_arg;
   uint32_t width, height;
   int LAYER=1;
   int s;

   if(dinit) return NULL;
   tr_set_text(string);
   alpha = 0xff;
   if(showing) {
       return NULL;
   }
   showing = 1;

   s = graphics_get_display_size(0, &width, &height);
   assert(s == 0);
   if(!img) {
	   s = gx_create_window(0, width, height, GRAPHICS_RESOURCE_RGBA32, &img);
	   assert(s == 0);
   }

   // transparent before display to avoid screen flash
   graphics_resource_fill(img, 0, 0, width, height, GRAPHICS_RGBA32(0,0,0,0x00));

   graphics_display_resource(img, 0, LAYER, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 1);

   while (alpha > 0) {
      graphics_resource_fill(img, 0, 0, width, height, GRAPHICS_RGBA32(0,0,0,0));
      // draw the subtitle text
      sem_wait(&sem);
      char *textc = strdup(text);
      sem_post(&sem);
      render_subtitle(img, text_size, textc);
      free(textc);
      graphics_update_displayed_resource(img, 0, 0, 0, 0);
      alpha -= 1;
   }
   showing = 0;
   graphics_display_resource(img, 0, LAYER, 0, 0, GRAPHICS_RESOURCE_WIDTH, GRAPHICS_RESOURCE_HEIGHT, VC_DISPMAN_ROT0, 0);
   return NULL;
}

void tr_show_thread(char * string) {
	pthread_create(&thread, NULL, &tr_show, strdup(string));
}

#endif