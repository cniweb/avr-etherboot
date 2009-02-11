/***************************************************************************
 *            ethernet.h
 *
 *  Sat Jun  3 14:57:38 2006
 *  Copyright  2006  User
 *  Email
 ****************************************************************************/

#ifndef __ETHERNET_H__

#include "enc28j60.h"

extern unsigned char ethernetbuffer[];
extern unsigned char ethernetbuffer_send[];
//extern unsigned char UDPRxBuffer[];

/* debugging jan
#define LEDON PORTD  |= (1<<(PD2))
#define LEDOFF PORTD &= ~(1<<(PD2))
#define LEDTOGGLE if (PORTD & (1<<PD2)) {LEDOFF;} else {LEDON;}

#define LEDTOGGLE0 if (PORTA & (1<<PA0)) {PORTA &= ~(1<<PA0);} else {PORTA |= (1<<(PA0));}
#define LEDTOGGLE1 if (PORTA & (1<<PA1)) {PORTA &= ~(1<<PA1);} else {PORTA |= (1<<(PA1));}
#define LEDON2 PORTA |= (1<<(PA2));
*/

#define __ETHERNET_H__
void ethernet(void);
extern unsigned char maMac[6];

void MakeETHheader (unsigned char * MACadress , unsigned char * buffer );

#define ETHERNET_MIN_PACKET_LENGTH	0x3C
#define ETHERNET_HEADER_LENGTH	0x0E

#define POLLINGMODE		0
#define INTERRUPTMODE	1

struct ETH_header {
	unsigned char ETH_destMac[6];	
	unsigned char ETH_sourceMac[6];
	unsigned int ETH_typefield;
};

#endif
