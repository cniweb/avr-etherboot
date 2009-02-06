/*! \file "timer1.c" \brief Timer 1 als Zeitbasis benutzen */
//***************************************************************************
//*            timer.c
//*
//*  Sun Jul 16 12:52:34 2006
//*  Copyright  2006  sharandac
//*  Email sharandac(at)snafu.de
//****************************************************************************/
///	\ingroup hardware
///	\defgroup TIMER1 Timer 1 als Zeitbasis benutzen (timer1.c)
///	\code #include "timer1.h" \endcode
///	\par Uebersicht
///		Timer 1 als Zeitbasis fuer igene Programme benutzen. Es koennen damit Timeout
/// Counter gebaut werden, oder beim Funktionen mit in den Timerinterrupt enigebunden werden.
//****************************************************************************/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
//@{
#include <avr/interrupt.h>
#include <avr/io.h>
#include "timer1.h"

TIMER1_CALLBACK_FUNC TIMER1_msec_CallbackFunc;

/*! \brief Die Timer1 init Funktion
 */
void timer1_init(void)
{

	cli();

	// Alle Callbackeinträge löschen
	TIMER1_msec_CallbackFunc = NULL;
	
	// jk 29.6.2007: Umgestellt von Prescaler 1024 auf Prescaler 64 wegen Genauigkeit
	TCCR1B |= ( 0<<CS12 ) | ( 1<<CS11 ) | ( 1<<CS10 );

	TCNT1 = 65535 - ( ( F_CPU / 64 ) / SECDIV ) ;
		

	// Timer1 einstellungen setzen
	#ifdef __AVR_ATmega2561__
		TIMSK1 |= ( 1<<TOIE1 );
	#else
		#ifdef __AVR_ATmega644__
			TIMSK1 |= ( 1<<TOIE1 );
		#else
			TIMSK |= ( 1<<TOIE1 );
		#endif

	#endif

	sei();
}

void timer1_deinit(void)
{
	#ifdef __AVR_ATmega2561__
		TIMSK1 &= ~( 1<<TOIE1 );
	#else
		#ifdef __AVR_ATmega644__
			TIMSK1 &= ~( 1<<TOIE1 );
		#else
			TIMSK &= ~( 1<<TOIE1 );
		#endif

	#endif
}
	
/*******************************************************************************
 Die ISR für Timer1
*******************************************************************************/
ISR(TIMER1_OVF_vect)
{
	
	unsigned char tmp_sreg;  	// temporaerer Speicher fuer das Statusregister
	tmp_sreg = SREG;   			// Statusregister (also auch das I-Flag darin) sichern

	cli();             			// Interrupts global deaktivieren

    TCNT1 = 65535 - ( ( F_CPU / 64 ) / SECDIV - 1) ;

	if (TIMER1_msec_CallbackFunc != NULL )
		TIMER1_msec_CallbackFunc();

	SREG = tmp_sreg;    		// Status-Register wieder herstellen
                      			// somit auch das I-Flag auf gesicherten Zustand setzen
}

