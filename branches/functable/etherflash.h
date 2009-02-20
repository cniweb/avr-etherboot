#ifndef ETHERFLASH_H
#define ETHERFLASH_H

#include <avr/io.h>

struct UDP_SOCKET {
	unsigned long DestinationIP;
	uint16_t SourcePort;
	uint16_t DestinationPort;
	uint16_t Bufferfill;
	uint16_t DataStartOffset;
	uint16_t BlockNumber;
	uint8_t lineBufferIdx;
};

struct TFTP_RESPONSE
{
    uint16_t op;		//1		Message op code / message type
						//		1 = RRQ, 2 = WRQ, 3 = DATA, 4 = ACK, 5 = ERR
	union
	{
		uint16_t blockNumber;
		uint16_t errCode;
	};
	union
	{
		uint8_t *data;
		char *errMsg;
	};
};

#define TFTP_OP_DATA 0x0300
#define TFTP_OP_ERR  0x0500

#define TFTP_TIMEOUT F_CPU / 3000 // ca. 5 seconds

int main(void) BOOTLOADER_SECTION;
void BootLoaderMain(void) BOOTLOADER_SECTION;
void tftp_get (void) BOOTLOADER_SECTION;
uint8_t hexToByte(uint8_t *buf, uint16_t idx) BOOTLOADER_SECTION;
void writeFLASHPage(uint32_t currentAddress) BOOTLOADER_SECTION;
void processLineBuffer(uint8_t bytes) BOOTLOADER_SECTION;
void jumpToApplication(void) BOOTLOADER_SECTION;



#define pApplication()     asm volatile ("call 0x00000"::)

#if defined (__AVR_ATmega2561__)
	#define pBootloader()      asm volatile ("call 0x3e000"::)
#endif
#if defined (__AVR_ATmega32__)
	#define pBootloader()      asm volatile ("call 0x7000"::)
#endif
#if defined (__AVR_ATmega644__)
	#define pBootloader()      asm volatile ("call 0xF000"::)
#endif


#endif
