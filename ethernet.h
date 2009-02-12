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

#define __ETHERNET_H__
void ethernet(void);

void MakeETHheader (unsigned char * MACadress);

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
