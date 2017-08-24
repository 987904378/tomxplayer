//
//  log.h
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

#ifndef LOG_H
#define LOG_H
#include <stdarg.h>
#include <stdio.h>

#define LOGE(tag, fmt, ...) fprintf(stderr,"E::%s::",tag) ; fprintf(stderr, fmt,__VA_ARGS__); fprintf(stderr, "\n")

#ifdef DEBUG
#define LOGD(tag, fmt, ...) fprintf(stderr,"D::%s::",tag) ; fprintf(stderr, fmt,__VA_ARGS__); fprintf(stderr, "\n")
#define LOGW(tag, fmt, ...) fprintf(stderr,"W::%s::",tag) ; fprintf(stderr, fmt,__VA_ARGS__); fprintf(stderr, "\n")
#define LOGI(tag, fmt, ...) fprintf(stderr,"I::%s::",tag) ; fprintf(stderr, fmt,__VA_ARGS__); fprintf(stderr, "\n")
#else
#define LOGD(tag, fmt, ...) while(0)
#define LOGW(tag, fmt, ...) while(0)
#define LOGI(tag, fmt, ...) while(0)
#endif

#endif
