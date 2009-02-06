#ifndef _BEEP_H
	#define _BEEP_H

#include <inttypes.h>
#include <avr/io.h>

#define BEEP_PORT PORTD
#define BEEP_DDR DDRD
#define BEEP_PIN PD4

void BEEP_init (void);
void BEEP_beep (uint8_t piCount, uint16_t piDurationMS, uint16_t piDelayMS);

#endif
