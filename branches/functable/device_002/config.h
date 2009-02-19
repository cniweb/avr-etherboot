#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>
#include <avr/eeprom.h>
#include <avr/boot.h>

#undef BOOTLOADER_SECTION
#define BOOTLOADER_SECTION
#define APP_SECTION_ENC __attribute__ ((section (".app_section_enc")))
#define APP_SECTION_DHCP __attribute__ ((section (".app_section_dhcp")))

#define	MYMAC1 0x02
#define	MYMAC2 0x01
#define	MYMAC3 0x02
#define	MYMAC4 0x03
#define	MYMAC5 0x04
#define	MYMAC6 0x99

#if defined (__AVR_ATmega32__)
	#define MTU_SIZE 600
#endif

#if defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__)
	#define MTU_SIZE 1200
#endif

#if defined (__AVR_ATmega128__)
	#define MTU_SIZE 1200
#endif

/////////////////////////////// PIN SETUP ////////////////////////////////////////////

/*
// ATMEGA2561

//-> enc28j60
#define ENC28J60_CONTROL_PORT	PORTG
#define ENC28J60_CONTROL_DDR	DDRG
#define ENC28J60_CONTROL_CS	3
#define ENC28J60_INT_PORTBIT    6

// -> SPI
#define SPI_PORT		PORTB
#define SPI_DDR			DDRB
#define SS				PB0
#define MISO			PB3
#define MOSI			PB2
#define SCK				PB1
*/

// ATMEGA32
	
#define USE_DHCP		0

#define DEBUG_AV		1
	
//-> enc28j60
#define USE_ENC28J60	1

// set these so that it reflects your hardware
// Net-IO board from Pollin
// #define ENC28J60_CONTROL_DDR	DDRB
// #define ENC28J60_CONTROL_PORT	PORTB

// Board Jan
#define ENC28J60_CONTROL_DDR	DDRD
#define ENC28J60_CONTROL_PORT	PORTD
		
#define ENC28J60_PIN_SCK	7
#define ENC28J60_PIN_MISO	6
#define ENC28J60_PIN_MOSI	5

// Net-IO board from Pollin
//#define ENC28J60_PIN_CS		4

// Board Jan
#define ENC28J60_PIN_CS		4


// -> SPI
#define SPI_PORT	PORTB
#define SPI_DDR		DDRB
#define MISO		ENC28J60_PIN_MISO
#define MOSI		ENC28J60_PIN_MOSI
#define SCK			ENC28J60_PIN_SCK


/////////////////////////////// HELPERS ////////////////////////////////////////////
//DO NOT CHANGE ANYTHING BELOW!

//convert config ip to long:
#define IP(a0,a1,a2,a3) ((((unsigned long)(a3))<<24)|(((unsigned long)(a2))<<16)|(((unsigned long)(a1))<<8)|(unsigned long)(a0))

/*#define NIC_IP_ADDRESS \
				(((uint32_t)NIC_IP_ADDRESS_0)<<24 | \
				((uint32_t)NIC_IP_ADDRESS_1)<<16 | \
				((uint32_t)NIC_IP_ADDRESS_2)<< 8 | \
				((uint32_t)NIC_IP_ADDRESS_3))

#define NIC_IP_NETMASK \
				(((uint32_t)NIC_IP_NETMASK_0)<<24 | \
				((uint32_t)NIC_IP_NETMASK_1)<<16 | \
				((uint32_t)NIC_IP_NETMASK_2)<< 8 | \
				((uint32_t)NIC_IP_NETMASK_3))


#define NIC_GATEWAY_IP_ADDRESS \
				(((uint32_t)NIC_GATEWAY_IP_ADDRESS_0)<<24 | \
				((uint32_t)NIC_GATEWAY_IP_ADDRESS_1)<<16 | \
				((uint32_t)NIC_GATEWAY_IP_ADDRESS_2)<< 8 | \
				((uint32_t)NIC_GATEWAY_IP_ADDRESS_3))

#define NTP_SERVER_IP_ADDRESS \
				(((uint32_t)NTP_SERVER_IP_ADDRESS_0)<<24 | \
				((uint32_t)NTP_SERVER_IP_ADDRESS_1)<<16 | \
				((uint32_t)NTP_SERVER_IP_ADDRESS_2)<< 8 | \
				((uint32_t)NTP_SERVER_IP_ADDRESS_3))
*/

#define lo8(x) ((x   )&0xFF)
#define hi8(x) (((x)>>8)&0xFF)


// Debugging
#if DEBUG_AV

#include <avr/pgmspace.h>

void sendchar (unsigned char Zeichen);
void puthexbyte(uint8_t bt);
void putstring (unsigned char *string);
void putPGMstring(const char *progmem_s);
#define putpgmstring(__s) putPGMstring(PSTR(__s))
#endif

#endif


