#ifndef ETHERFLASH_H
#define ETHERFLASH_H

#include <avr/io.h>

struct UDP_SOCKET {
	unsigned long DestinationIP;
	unsigned int SourcePort;
	unsigned int DestinationPort;
	unsigned int Bufferfill;
	unsigned int DataStartOffset;
	uint8_t lineBufferIdx;
};

void tftp_get (void) SEC_BOOTLOADER;
uint8_t hexToByte(uint8_t *buf, uint16_t idx) SEC_BOOTLOADER;
void writeFLASHPage(uint32_t currentAddress) SEC_BOOTLOADER;
void processLineBuffer(uint8_t bytes) SEC_BOOTLOADER;
void jumpToApplication(void) SEC_BOOTLOADER;



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
