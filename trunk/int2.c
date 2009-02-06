/*! \file int2.c \brief Int2-Haendler */
//***************************************************************************
//*            int2.c
//*
//*  Tue Aug 15 19:59:38 2006
//*  Copyright  2006  sharandac
//*  Email sharandac[at]snafu.de
//****************************************************************************/
///	\ingroup hardware
///	\defgroup INT2 Funktionen fuer dn INT2 (int2.c)
///	\code #include "int2.h" \endcode
///	\par Uebersicht
///		Funktionen um mit den INT2 zu arbeiten.
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
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "int2.h"

unsigned char interrupt;

#ifdef __AVR_ATmega2561__
	struct INT2_CB
	{
		uint8_t piBitMask;
		INT2_CALLBACK_FUNC ppFunc;
	};
	
	struct INT2_CB maINT2_CallbackFunc [MAX_INT2_CALLBACKS];
#else
#ifdef __AVR_ATmega644__
	struct INT2_CB
	{
		uint8_t piBitMask;
		INT2_CALLBACK_FUNC ppFunc;
	};
	
	struct INT2_CB maINT2_CallbackFunc [MAX_INT2_CALLBACKS];
#else
	INT2_CALLBACK_FUNC maINT2_CallbackFunc [MAX_INT2_CALLBACKS];
#endif
#endif


/* -----------------------------------------------------------------------------------------------------------*/
/*! Die Init fuer die INT2.
 */
/* -----------------------------------------------------------------------------------------------------------*/
#ifdef __AVR_ATmega2561__
	void INT_init (uint8_t piINTBitMask)
#else
#ifdef __AVR_ATmega644__
	void INT_init (uint8_t piINTBitMask)
#else
	void INT2_init (void)
#endif
#endif
	{
		volatile uint8_t liSREG;
		liSREG = SREG;
		cli();
		
		unsigned char i;
		// Alle Callbackeinträge löschen
		#ifdef __AVR_ATmega2561__
			for ( i = 0 ; i < MAX_INT2_CALLBACKS ; i++ ) maINT2_CallbackFunc[i].ppFunc = NULL;
		#else
		#ifdef __AVR_ATmega644__
			for ( i = 0 ; i < MAX_INT2_CALLBACKS ; i++ ) maINT2_CallbackFunc[i].ppFunc = NULL;
		#else
			for ( i = 0 ; i < MAX_INT2_CALLBACKS ; i++ ) maINT2_CallbackFunc[i] = NULL;
		#endif
		#endif

		#ifdef __AVR_ATmega2561__
			// Default sense control = "falling edge"
			EICRA = (1<<ISC01) | (1<<ISC11) | (1<<ISC21) | (1<<ISC31);
			EICRB = (1<<ISC41) | (1<<ISC51) | (1<<ISC61) | (1<<ISC71);
			
			// enable interrupts for selected signals
			EIMSK = piINTBitMask;
			
		#else
			#ifdef __AVR_ATmega644__
				// Default sense control = "falling edge"
				EICRA = (1<<ISC01) | (1<<ISC11) | (1<<ISC21);
				
				// enable interrupts for selected signals
				EIMSK = piINTBitMask;
			#else
				#ifdef BREADBOARD
					// Enable Interrupt for INT1
					// On ATMEGA32 use INT1 in falling Edge Mode
					MCUCR |= (1<<ISC11);
					GICR |= (1<<INT1);
				#else
					MCUCSR &= ~(1<<ISC2);
					GICR |= (1<<INT2);
				#endif
			#endif
		#endif
		// Interruptfreigabe für INT2
		//GICR |= 0x20;
		
		// Clear Interrupts
		EIFR |= (1<<INTF2) | (1<<INTF1) | (1<<INTF0);
		
		// restore SREG
		SREG = liSREG;
		
	}
			
		
#ifdef __AVR_ATmega2561__

	ISR(INT0_vect)
	{
		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob dei Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT0)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}
	ISR(INT1_vect)
	{
		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob dei Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT1)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}	
	ISR(INT2_vect)
	{
		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob dei Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT2)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}	
	ISR(INT3_vect)
	{
		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob dei Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT3)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}	
	ISR(INT4_vect)
	{
		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob dei Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT4)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}
	ISR(INT5_vect)
	{
		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob dei Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT5)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}	
	ISR(INT6_vect)
	{
		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob dei Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT6)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}	
	ISR(INT7_vect)
	{
		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob dei Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT7)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}	
	

#else
#ifdef __AVR_ATmega644__
	ISR(INT0_vect)
	{
		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob dei Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT0)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}
	ISR(INT1_vect)
	{
		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob dei Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT1)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}	
	
	ISR(INT2_vect)
	{

		uint8_t i;
		for (i=0; i< MAX_INT2_CALLBACKS; i++)
		{	
			// prüfen, ob die Funktion aktiv ist und der Index passt
			if ((maINT2_CallbackFunc[i].piBitMask & (1<<INT2)) && maINT2_CallbackFunc[i].ppFunc != NULL)
			{
				// Funktion aufrufen
				maINT2_CallbackFunc[i].ppFunc();
			}
		}
	}	

#else

	ISR(INT2_vect)
		{
			unsigned char i;
			//putstring ("INT2_ISR ");
			for (i = 0; i < MAX_INT2_CALLBACKS; i++ ) if (maINT2_CallbackFunc[i] != NULL ) maINT2_CallbackFunc[i]();
		}
			
	ISR(INT1_vect)
		{
			unsigned char i;
			for (i = 0; i < MAX_INT2_CALLBACKS; i++ ) if (maINT2_CallbackFunc[i] != NULL ) maINT2_CallbackFunc[i]();
		}	

#endif
#endif

#ifdef __AVR_ATmega2561__
	uint8_t INT_enabledBitMask (void)
	{
		return (EIMSK);
	}
	
	void INT_enable_mask (uint8_t piINTBitMask)
	{
		EIMSK = piINTBitMask;
	}

	void INT_enable_bit (uint8_t piINTBit)
	{
		EIMSK |= (1<<piINTBit);
	}

	void INT_disable_bit(uint8_t piINTBit)
	{
		EIMSK &= ~(1<<piINTBit);
	}
#else
#ifdef __AVR_ATmega644__
	uint8_t INT_enabledBitMask (void)
	{
		return (EIMSK);
	}
	
	void INT_enable_mask (uint8_t piINTBitMask)
	{
		EIMSK = piINTBitMask;
	}

	void INT_enable_bit (uint8_t piINTBit)
	{
		EIMSK |= (1<<piINTBit);
	}

	void INT_disable_bit(uint8_t piINTBit)
	{
		EIMSK &= ~(1<<piINTBit);
	}
#else
	void INT2_disable (void)
	{
		#ifdef BREADBOARD
		GICR &= ~(1<<INT1);
		#else
		GICR &= ~(1<<INT2);
		#endif
		// GICR &= 0xdf;
	}
	
	void INT2_enable (void)
	{
		#ifdef BREADBOARD
		GICR |= (1<<INT1);
		#else
		GICR |= (1<<INT2);
		#endif
		//GICR |= 0x20;
	}

#endif
#endif

#ifdef __AVR_ATmega2561__

	unsigned char INT_RegisterCallbackFunction (uint8_t piINTBitMask, uint8_t piINTType, INT2_CALLBACK_FUNC pFunc)
	{
		unsigned char i;
		
		for (i = 0; i < MAX_INT2_CALLBACKS; i++ ) 
		{
			if (maINT2_CallbackFunc[i].ppFunc == pFunc ) 
			{
				return TRUE;
			}
		}
		
		for (i = 0; i < MAX_INT2_CALLBACKS; i++ )
		{
			if (maINT2_CallbackFunc[i].ppFunc == NULL)
			{
				maINT2_CallbackFunc[i].ppFunc = pFunc;
				maINT2_CallbackFunc[i].piBitMask = piINTBitMask;
				return TRUE;
			}
		}
		return FALSE;
	}
	
	unsigned char INT_RemoveCallbackFunction (INT2_CALLBACK_FUNC pFunc)
	{
		unsigned char i;
		
		for (i = 0; i < MAX_INT2_CALLBACKS; i++ )
		{
			if (maINT2_CallbackFunc[i].ppFunc == pFunc )
			{
				maINT2_CallbackFunc[i].ppFunc = NULL;
				return TRUE;
			}
		}
		return FALSE;
	}

#else
#ifdef __AVR_ATmega644__
	unsigned char INT_RegisterCallbackFunction (uint8_t piINTBitMask, uint8_t piINTType, INT2_CALLBACK_FUNC pFunc)
	{
		unsigned char i;
		
		for (i = 0; i < MAX_INT2_CALLBACKS; i++ ) 
		{
			if (maINT2_CallbackFunc[i].ppFunc == pFunc ) 
			{
				return TRUE;
			}
		}
		
		for (i = 0; i < MAX_INT2_CALLBACKS; i++ )
		{
			if (maINT2_CallbackFunc[i].ppFunc == NULL)
			{
				maINT2_CallbackFunc[i].ppFunc = pFunc;
				maINT2_CallbackFunc[i].piBitMask = piINTBitMask;
				return TRUE;
			}
		}
		return FALSE;
	}
	
	unsigned char INT_RemoveCallbackFunction (INT2_CALLBACK_FUNC pFunc)
	{
		unsigned char i;
		
		for (i = 0; i < MAX_INT2_CALLBACKS; i++ )
		{
			if (maINT2_CallbackFunc[i].ppFunc == pFunc )
			{
				maINT2_CallbackFunc[i].ppFunc = NULL;
				return TRUE;
			}
		}
		return FALSE;
	}
#else

	unsigned char INT2_RegisterCallbackFunction (INT2_CALLBACK_FUNC pFunc)
	{
		unsigned char i;
		
		for (i = 0; i < MAX_INT2_CALLBACKS; i++ ) 
		{
			if (maINT2_CallbackFunc[i] == pFunc)
			{
				return TRUE;
			}
		}
		
		for (i = 0; i < MAX_INT2_CALLBACKS; i++ )
		{
			if (maINT2_CallbackFunc[i] == NULL)
			{
				maINT2_CallbackFunc[i] = pFunc;
				return TRUE;
			}
		}
		return FALSE;
	}

	unsigned char INT2_RemoveCallbackFunction (INT2_CALLBACK_FUNC pFunc)
	{
		unsigned char i;
		
		for (i = 0; i < MAX_INT2_CALLBACKS; i++ )
		{
			if (maINT2_CallbackFunc[i] == pFunc)
			{
				maINT2_CallbackFunc[i] = NULL;
				return TRUE;
			}
		}
		return FALSE;
	}
#endif
#endif

//}@
