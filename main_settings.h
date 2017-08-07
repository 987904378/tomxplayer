//
//  main_settings.h
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

#ifndef MAIN_SETTINGS_H
#define MAIN_SETTINGS_H
#include <stdlib.h>
#include "settings.h"

setting_t cont_pb;
setting_t stretch;
setting_t volume;
setting_t audio_out;
setting_t file_types;
#ifndef NO_OSD
setting_t osd_enable;
setting_t osd_textsize;
setting_t osd_textsize_percent;
#endif
setting_t win_trans_unfocus;
setting_t win_trans_alpha;
setting_t omx_extra_args;
#ifdef POLLWINPOS
setting_t border_offset;
setting_t title_bar_offset;
#endif
setting_t arb_x_offset;
setting_t arb_y_offset;

#endif
