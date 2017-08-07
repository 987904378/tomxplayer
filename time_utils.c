//
//  time_utils.c
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

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "time_utils.h"

#define TAG "time_utils"

/* Convert microseconds to formated time HH:MM:SS
 */
int ms_to_time(int64_t *ms1, char **dest) {
	int64_t ms = *ms1;
	if(ms >= 86400000000) {
		fprintf(stderr, "Can not convert microseconds >= 24 hours\n");
		return 1;
	}
	if(ms == 0) {
		asprintf(dest, "%s", "00:00:00");
		return 0;
	}
	int64_t hours = ms / 1000 / 1000 / 60 / 60;
	hours = floor(hours);
	ms -= hours * 60 * 60 * 1000 * 1000;
	int64_t minutes = ms / 1000 / 1000 / 60;
	minutes = floor(minutes);
	ms -= minutes * 60 * 1000 * 1000;
	int64_t seconds = ms / 1000 / 1000;
	seconds = floor(seconds);

	asprintf(dest,"%.2lld:%.2lld:%.2lld",hours,minutes,seconds);
	return 0;
}

# if 0
int main (int argc, char * argv[]) {
	if(argc < 2) {
	fprintf(stderr, "Forget something?\n");
		return 1;
	}
	char *dest;
	unsigned long ms = (unsigned long)atol(argv[1]);

	if(!ms_to_time(ms,dest))
		printf("%s\n",dest);
	return 0;
}
#endif
