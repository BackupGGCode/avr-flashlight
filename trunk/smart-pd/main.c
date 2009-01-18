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

/*
 * Software for Nitecore 'SmartPD' D10 / EX10 torches
 *
 * ******** Fuses ********
 * Target uC: Atmel Tiny13 @ 9.6MHz
 * 
 * ******** Fuses ********
 * Recommended fuse settings:
 * BODLEVEL 2.7V
 * SPIEN
 * SUT_CKSEL Int RC Osc. 9.6MHz, Start-up time: 14 CK + 0ms
 *
 * Writing fuses address 0 to 1.. 0x72, 0xFB .. OK!
 *
 * ******** Compiler ********
 * This was written with gcc version 4.3.2 (WinAVR 20081124rc3)
 * http://winavr.sourceforge.net/
 *
 */
 
#include "main.h"

int main( void )
{
	hardware_init();

	while(1)
	{
		one_button_statemachine();
	}
}

ISR(PCINT0_vect)
{
	asm volatile("nop\n\t");
}

void one_button_statemachine(void)
{
	static system_state_enum system_state = sys_idle;
	static bool up_down=true;
	static unsigned char bindex;
	unsigned char i;
	switch(system_state)
	{
	case sys_idle:
		//load stored value for state machine
		bindex=eeprom_read_byte((const uint8_t *)eeprom_bindex);

		//if bindex is invalid override...
		if(bindex >= NUM_BRIGHT_LEVELS)
		{
			bindex=0;
		}
		///\todo: implement wear-levelling, else we will eventually kill that byte of eeprom...

		//if we are just powered we always go ON
		up_down = ( bindex < ( NUM_BRIGHT_LEVELS - 1 ) ) ? true : false;//ensure we ramp the right direction
		system_state = sys_on;
		break;

	case sys_to_on:
		//check for a held button, user might want full brightness
		TCCR0A |= (1<<COM0B1);
		OCR0B = pgm_read_byte(&brightness[bindex]);

		//400ms
		if( delay_piston_pressed(40) )
		{
			//has time expired? user still holding piston{
			bindex = NUM_BRIGHT_LEVELS-1;
			up_down = false;//down as we are max
			OCR0B = pgm_read_byte(&brightness[bindex]);//update LED early, ensures we are responsive to the users input
			while(PISTON_PRESSED);//wait for sw to release so we don't go straight in to ramping
		}
		_delay_ms(BOUNCE_TIME);//de-bounce any activity

		system_state = sys_on;
		break;

	case sys_on:
		//on at previous brightness
		OCR0B = pgm_read_byte(&brightness[bindex]);
		if(PISTON_PRESSED)
		{
			_delay_ms(BOUNCE_TIME);//debounce
			//time the press, presses of less than 200ms turns off
			if( !delay_piston_pressed(38) )
			{
				//switch was released early, user wants to switch off?
				//disable the pwm output
				OCR0B=0;
				TCCR0A &= ~(1<<COM0B1);
				_delay_ms(BOUNCE_TIME);//debounce
				system_state=sys_off_think;
				break;
			}

			//ramp brightness!
			system_state = sys_ramp_brightness;
			break;
		}
		break;

	case sys_off:

		//turn off hardware

		//sleep here
		// Enable sleep mode
        MCUCR |= (1<<SE);

        sleep_cpu();     //we can only be woken by pcint

        // Disable sleep mode
        MCUCR &= ~(1<<SE);

		//awake? check source
		if(PISTON_PRESSED)
		{
			_delay_ms(BOUNCE_TIME);//bounce
			TCCR0A |= (1<<COM0B1);
			//reload the brightness from eeprom
			bindex = eeprom_read_byte((const uint8_t *)eeprom_bindex);
			OCR0B = pgm_read_byte(&brightness[bindex]);

			up_down = ( bindex < ( NUM_BRIGHT_LEVELS - 1 ) ) ? true : false;

			while(PISTON_PRESSED);
			_delay_ms(BOUNCE_TIME);//bounce
			system_state = sys_on;
			break;
		}

		//we repeat if we get here
		break;

	case sys_off_think:
		//decide if we actually want to go off...

		//make sure the button releases within 200ms, then pressed again within 200ms for instant low

		//pressed again?
		for( i = 140; (i != 0) && (!PISTON_PRESSED); i--)
		{
			_delay_ms(1);
		}

		if( i != 0 )
		{
			//button was pressed within the time limit
			//min brightness
			bindex=0;
			up_down=true;//go up next time
			system_state=sys_to_on;
			break;
		}

		//timeout! just sleep
		system_state=sys_off;
		break;

	case sys_ramp_brightness:
		if( !PISTON_PRESSED )
		{
			//piston has been released, save!
			eeprom_write_byte((uint8_t *)eeprom_bindex,bindex);
			up_down^=true;
			_delay_ms(BOUNCE_TIME);//debounce
			system_state = sys_on;
			break;
		}
		_delay_ms(100);//sets the rate we ramp levels

		if( up_down && ( bindex < ( NUM_BRIGHT_LEVELS - 1 ) ) )
		{
			//go up!
			bindex++;
		}
		if( (!up_down) && ( bindex > 0 ) )
		{
			bindex--;
		}

		OCR0B = pgm_read_byte(&brightness[bindex]);
		//stay here
		break;

	default:
		system_state = sys_idle;
		break;
	}
}


