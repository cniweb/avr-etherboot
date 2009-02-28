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

int main(void);
void BootLoaderMain(void);
void tftp_get (void) BOOTLOADER_SECTION;
uint8_t hexToByte(uint8_t *buf, uint16_t idx);
void writeFLASHPage(uint32_t currentAddress);
void processLineBuffer(uint8_t bytes) BOOTLOADER_SECTION;
void jumpToApplication(void) BOOTLOADER_SECTION;


#if FLASHEND < 0x3fff
// ATmega8
#define pApplication()     asm volatile ("rcall 0x0000"::)
#define pBootloader()      asm volatile ("rcall FLASHEND-BLSECSTRT"::)
#else
#define pApplication()     asm volatile ("call 0x00000"::)
#define pBootloader()      asm volatile ("call FLASHEND-BLSECSTRT"::)
#endif

#endif
