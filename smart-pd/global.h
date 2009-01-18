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
#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/pgmspace.h>

#include "version.h"//automatically generated version information

enum CORE_SPEEDS
{
	CORE_DIV_1,
	CORE_DIV_2,
	CORE_DIV_4,
	CORE_DIV_8,
	CORE_DIV_16,
	CORE_DIV_32,
	CORE_DIV_64,
	CORE_DIV_128,
	CORE_DIV_256
};

// System clock definitions

#define Xtal					9600000
#define CORE_DIVIDER			CORE_DIV_1						//must be in CORE_SPEEDS enum
#define MAIN_DIVIDE_RATIO		_BV( CORE_DIVIDER )
#define Fosc					( Xtal/MAIN_DIVIDE_RATIO )		// Main oscillator Hz
#ifdef  F_CPU
	#undef F_CPU //stop command-line f_cpu
	#warning ignoring command-line F_CPU!
#endif
#define F_CPU 					( (unsigned long)Fosc )

#include <util/delay.h>

enum{B0_NC, B1_FET, B2_SW, B3_NC, B4_NC, B5_NC};


enum bool_type
{
	false = (1==0),
	true  = (1==1)
};
typedef enum bool_type bool;

#define RESET()			__indirect_jump_to      ( RESET_vect )

#define BOUNCE_TIME 	( 5 )
#define INPUT_SW 		( B2_SW )

#define PISTON_PRESSED	(!(PINB&(1<<INPUT_SW)))

//eeprom storage location
#define eeprom_bindex 	0

#endif

/*e0f*/
