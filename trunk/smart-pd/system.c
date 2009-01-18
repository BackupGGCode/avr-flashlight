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
#include "global.h"
#include "system.h"

static void SetPrescalerRegister      ( enum CORE_SPEEDS );

void hardware_init(void)
{
	///\brief initialises minimal hardware for uC
	//clock div
	SetPrescalerRegister(CORE_DIVIDER);

	DDRB = ( 1 << B1_FET );
	PORTB = ~DDRB;

	//use fast pwm mode con ccpa
	TCCR0A = (1<<COM0B1)|(1<<WGM01)|(1<<WGM00);
	TCCR0B = 1;//clock directly
	OCR0B = 0x00;

	//pin change interrupt
	MCUCR |= (1<<ISC00);
	GIMSK = (1<<PCIE);
	PCMSK = (1<<INPUT_SW);

	//set the sleep mode we want:
	MCUCR |= (1<<SM1);//power-down
	sei();
}

bool delay_piston_pressed(uint8_t d)
{
	//delay d (in 10ms steps), return the logic state of the piston,
	//ie true if pressed after time elapses, false if released, exits early if released
	while(d--)
	{
		_delay_ms(10);
		if(!PISTON_PRESSED)
		{
			return false;
		}
	}
	return true;
}

static void SetPrescalerRegister( enum CORE_SPEEDS PrescalerValue )
{
	///\brief sets the uC prescaler to prescaler value (isnt that kinda obvious??)
	CLKPR=_BV(CLKPCE);			//prepare the prescale register for our value
	CLKPR=PrescalerValue;		//set it!
}

/*e0f*/
