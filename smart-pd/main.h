/*
	Copyright (C) 2008 Duncan Brass
*/

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __MAIN_H__
#define __MAIN_H__

#include "global.h"
#include "system.h"


	typedef enum
	{
		sys_idle,
		sys_to_on,
		sys_on,
		sys_off,
		sys_off_think,
		sys_ramp_brightness,
		sys_max
	}system_state_enum;

	void one_button_statemachine(void);

	#define NUM_BRIGHT_LEVELS	50

	prog_uint8_t brightness[NUM_BRIGHT_LEVELS] = {0,1,2,3,4,6,7,9,11,13,15,18,21,24,27,30,34,37,41,45,49,54,58,63,68,73,79,84,90,96,102,109,115,122,129,136,143,150,158,166,174,182,190,199,208,217,226,235,245,255};

#endif
