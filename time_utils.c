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

/* Convert microseconds to formated time HH:MM:SS
 */
int ms_to_time(unsigned long ms, char **dest) {
	if(ms >= 86400000000) {
		fprintf(stderr, "Can not convert microseconds >= 24 hours\n");
		return 1;
	}
	long hours = ms / 1000 / 1000 / 60 / 60;
	hours = floor(hours);
	ms -= hours * 60 * 60 * 1000 * 1000;
	long minutes = ms / 1000 / 1000 / 60;
	minutes = floor(minutes);
	ms -= minutes * 60 * 1000 * 1000;
	long seconds = ms / 1000 / 1000;
	seconds = floor(seconds);

	asprintf(dest,"%.2ld:%.2ld:%.2ld",hours,minutes,seconds);
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
