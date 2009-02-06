#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdio.h>


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
//-> enc28j60
#define ENC28J60_CONTROL_PORT	PORTD
#define ENC28J60_CONTROL_DDR	DDRD
#define ENC28J60_CONTROL_CS	4
#define ENC28J60_INT_PORT		PORTB
#define ENC28J60_INT_DDR		DDRB
#define ENC28J60_INT_INTBIT		INT2
#define ENC28J60_INT_PORTBIT	2

// -> SPI
#define SPI_PORT		PORTB
#define SPI_DDR			DDRB
#define SS				PB4
#define MISO			PB6
#define MOSI			PB5
#define SCK				PB7

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

#ifndef sbi
	#define sbi(reg,bit)    reg |= (1<<(bit))
#endif

#ifndef cbi
	#define cbi(reg,bit)    reg &= ~(1<<(bit))
#endif
#ifndef MIN
	#define MIN(a,b)        ((a<b)?(a):(b))
#endif

#define lo8(x) ((x   )&0xFF)
#define hi8(x) (((x)>>8)&0xFF)


#endif

