#include "beep.h"
#include <util/delay.h>

uint8_t miCount;
uint16_t miDurationMS;
uint16_t miDelayMS;

void BEEP_init (void)
{
	// set Beep-PIN to output and drive low
	BEEP_DDR |= (1<<BEEP_PIN);
	BEEP_PORT &= ~(1<<BEEP_PIN);

}


void BEEP_beep (uint8_t piCount, uint16_t piDurationMS, uint16_t piDelayMS)
{
	
	// turn on Beeper
	BEEP_PORT |= (1<<BEEP_PIN);
	_delay_ms(2);
	BEEP_PORT &= ~(1<<BEEP_PIN);
	
}
