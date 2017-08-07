//
//  main_settings.c
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

#include "main_settings.h"

setting_t cont_pb = {
	"cont_pb",
	"Continuous Playback",
	"Playback",
	"",
	"",
	0,
	1, 0, 1,
	SETTINGS_TYPE_BOOL,
	1,
	"Loads all video and audio from the folder and loops a playlist.",
	NULL,
	NULL,
};

setting_t stretch = {
	"stretch",
	"Stretch Video",
	"Playback",
	"",
	"",
	0,
	0, 0, 1,
	SETTINGS_TYPE_BOOL,
	1,
	"Stretch the video to fit the client window.",
	NULL,
	NULL,
};

setting_t volume = {
	"volume",
	"Volume",
	"Audio",
	"",
	"",
	0,
	100, 0, 150,
	SETTINGS_TYPE_INT,
	0,
	"Hidden Setting to persist volume.",
	NULL,
	NULL,
};

setting_t audio_out = {
	"audio_out",
	"Audio Output",
	"Audio",
	"",
	"hdmi",
	4,
	0,0,0,
	SETTINGS_TYPE_STRING,
	1,
	"Select the audio output device. This does not take effect until the next video is loaded.",
	NULL,
	"hdmi\0local\0both\0alsa:default",
};

setting_t file_types = {
	"file_types",
	"Playlist File Types",
	"Playback",
	"",
	"mp4 | avi | divx | mpeg | mkv | mpg | 3g2 | 3gp | asf | flv | h264 | m2ts | m4v | mod | mov | tod | vob | webm | wmv",
	0,
	0,0,0,
	SETTINGS_TYPE_STRING,
	1,
	"A separated list of file extentions to include in building a playlist from a directory. Do not include '.'!",
	NULL,
	"",
};

#ifndef NO_OSD
setting_t osd_enable = {
	"osd_enable",
	"On-Screen Display",
	"On-Screen Display",
	"",
	"",
	0,
	1,0,1,
	SETTINGS_TYPE_BOOL,
	1,
	"Enable text to be displayed over the video when actions are performed.",
	NULL,
	"",
};

setting_t osd_textsize = {
	"osd_txtsize",
	"Text Size",
	"On-Screen Display",
	"",
	"",
	0,
	5,5,40,
	SETTINGS_TYPE_INT,
	1,
	"Size of the text to be displayed.",
	NULL,
	"",
};

setting_t osd_textsize_percent = {
	"osd_txtsize_percent",
	"Text Size As Percent",
	"On-Screen Display",
	"",
	"",
	0,
	1,0,1,
	SETTINGS_TYPE_BOOL,
	1,
	"Use 'Text Size' as a percentage of the render window height, dynamically sizing with it.",
	NULL,
	"",
};
#endif

setting_t win_trans_unfocus = {
	"win_trans_unfocus",
	"Unfocused Transparency",
	"Window",
	"",
	"",
	0,
	0,0,1,
	SETTINGS_TYPE_BOOL,
	1,
	"When another window has input focus, Make the render window semi-transparent. "
	"This allows menus and windows to be seen underneath.",
	NULL,
	"",
};

setting_t win_trans_alpha = {
	"win_trans_alpha",
	"Transparency Value",
	"Window",
	"",
	"",
	0,
	123,10,245,
	SETTINGS_TYPE_INT,
	1,
	"Scale of 10 to 245, 10 being almost completely transparent, and 245 being slightly transparent.",
	NULL,
	"",
};

setting_t omx_extra_args = {
	"omx_extra_args",
	"Exra Arguments for omxplayer",
	"Advanced",
	"",
	"",
	0,
	0,0,0,
	SETTINGS_TYPE_STRING,
	1,
	"Extra arguments to be passed to omxplayer. BE CAREFULL!. 'man omxplayer' for options.",
	NULL,
	"",
};

#ifdef POLLWINPOS

setting_t border_offset = {
	"border_offset",
	"Window Border Offset",
	"Advanced",
	"",
	"",
	0,
	2, 0, 65535,
	SETTINGS_TYPE_INT,
	1,
	"A x and y offset applied when the window is not maximized and not fullscreen.",
	NULL,
	"",
};

setting_t title_bar_offset = {
	"title_bar_offset",
	"Title Bar Offset",
	"Advanced",
	"",
	"",
	0,
	28,0 ,65535,
	SETTINGS_TYPE_INT,
	1,
	"Offset for the title bar (window decorations). A render window y offset applied when not fullscreen. This may need to be changed if system fonts sizes change.",
	NULL,
	"",
};

#endif

setting_t arb_x_offset = {
	"arb_x_offset",
	"Arbitrary x Offset",
	"Advanced",
	"",
	"",
	0,
	0,-65535,65535,
	SETTINGS_TYPE_INT,
	1,
	"A render window alignment x offset ment to handle overscan 'black bars' at the left and right of the sreen.",
	NULL,
	"",
};

setting_t arb_y_offset = {
	"arb_y_offset",
	"Arbitrary y Offset",
	"Advanced",
	"",
	"",
	0,
	0,-65535,65535,
	SETTINGS_TYPE_INT,
	1,
	"A render window alignment y offset ment to handle overscan 'black bars' at the top and bottom of the sreen.",
	NULL,
	"",
};
