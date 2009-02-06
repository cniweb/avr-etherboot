#ifndef ETHERFLASH_H
#define ETHERFLASH_H

#include <avr/io.h>

#define pApplication()     asm volatile ("call 0x00000"::)

#if defined (__AVR_ATmega2561__)
	#define pBootloader()      asm volatile ("call 0x3e000"::)
#endif
#if defined (__AVR_ATmega32__)
	#define pBootloader()      asm volatile ("call 0x7000"::)
#endif
#if defined (__AVR_ATmega644__)
	#define pBootloader()      asm volatile ("call 0x7000"::)
#endif


#endif
