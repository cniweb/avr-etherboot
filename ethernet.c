/***************************************************************************
 *            ethernet.c
 *
 *  Sat Jun  3 17:25:42 2006
 *  Copyright  2006  User
 *  Email
 ****************************************************************************/
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "ethernet.h"
#include "arp.h"
#include "enc28j60.h"
#include "ip.h"
#include "udp.h"
#include "config.h"
#include "eemem.h"

#include <stdio.h>


/*
 -----------------------------------------------------------------------------------------------------------
   Die Routine die die pakete nacheinander abarbeitet
------------------------------------------------------------------------------------------------------------*/

unsigned char ethernetbuffer[MTU_SIZE];

inline void ethernet(void)
{

	unsigned int packet_length;

	// hole ein Frame
	packet_length = ETH_PACKET_RECEIVE (MTU_SIZE, ethernetbuffer);

	// wenn Frame vorhanden packet_lenght != 0
	// arbeite so lange die Frames ab bis keine mehr da sind
	if (packet_length != 0)
	{

		struct ETH_header *ETH_packet; 		//ETH_struc anlegen
		ETH_packet = (struct ETH_header *) ethernetbuffer;
		switch ( ETH_packet->ETH_typefield ) // welcher type ist gesetzt
		{
			case 0x0608:
				arp (packet_length);
				break;

			case 0x0008:
				if (((struct IP_header *)&ethernetbuffer[ETHERNET_HEADER_LENGTH])->IP_Protocol == 0x11)
				{
					udp (packet_length);
				}
				break;
		}

	}
}



/* -----------------------------------------------------------------------------------------------------------
Erstellt den richtigen Ethernetheader zur passenden Verbindung die gerade mit TCP_socket gewählt ist
------------------------------------------------------------------------------------------------------------*/
void MakeETHheader (unsigned char * MACadress)
{
	struct ETH_header *ETH_packet; 		// ETH_struct anlegen
	ETH_packet = (struct ETH_header *) ethernetbuffer;

	unsigned int i;			

	ETH_packet->ETH_typefield = 0x0008;
	
	for ( i = 0 ; i < 6 ; i++ )
	{
		ETH_packet->ETH_sourceMac[i] = eeprom_read_byte(&enc28j60_config[ENC28J60_CONFIG_OFFSET_MAC + i*2]);
		ETH_packet->ETH_destMac[i] = MACadress[i];
	}

}
