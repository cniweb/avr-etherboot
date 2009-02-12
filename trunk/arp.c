/*! \file "arp.c" \brief Implementation of the arp protocol */
///	\ingroup network
///	\defgroup ARP ARP-Funktionen (arp.c)
///	\code #include "arp.h" \endcode
///	\code #include "ip.h" \endcode
///	\code #include "ethernet.h" \endcode
///	\par Uebersicht
//****************************************************************************/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
//@{
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>

#include "arp.h"
#include "ethernet.h"
#include "ip.h"
#include "config.h"
#include "eemem.h"

struct ARP_TABLE ARPtable[ MAX_ARPTABLE_ENTRYS ];
struct ARP_TABLE *ARP_table;

void arp (unsigned int packet_length)
{
	unsigned char i;

	struct ETH_header *ETH_packet;
	ETH_packet = (struct ETH_header *) ethernetbuffer; 
	struct ARP_header *ARP_packet;
	ARP_packet = (struct ARP_header *) &ethernetbuffer[ETHERNET_HEADER_LENGTH];
	switch ( ARP_packet->ARP_Opcode ) {
		
	case 0x0100:
		if ( ARP_packet->ARP_destIP != mlIP )
		{
			return;
		}

		ARP_packet->ARP_Opcode = 0x0200;
		
		// mac und ip des senders in ziel kopieren
		for ( i = 0; i < 10; i++ )
			ARP_packet->ARP_destMac[i] = ARP_packet->ARP_sourceMac[i]; // MAC und IP umkopieren
		
		// meine mac und ip als absender einsetzen
		for ( i = 0; i < 6 ; i++ )
			ARP_packet->ARP_sourceMac[i] = eeprom_read_byte(&enc28j60_config[ENC28J60_CONFIG_OFFSET_MAC + i*2]); // MAC einsetzen
		
		ARP_packet->ARP_sourceIP = mlIP ; // IP einsetzen
		
		// sourceMAC in destMAC eintragen und meine MAC in sourceMAC kopieren
		for (i = 0 ; i < 6 ; i++)
		{	
			ETH_packet->ETH_destMac[i] = ETH_packet->ETH_sourceMac[i];	
			ETH_packet->ETH_sourceMac[i] = eeprom_read_byte(&enc28j60_config[ENC28J60_CONFIG_OFFSET_MAC + i*2]);
		}
		
		enc28j60PacketSend (packet_length, ethernetbuffer);

		break;

	case 0x0200:
		ARP_table = &ARPtable[0];
		ARP_table->IP = ARP_packet->ARP_sourceIP;
		for ( i = 0 ; i < 6 ; i++ ) 
			ARP_table->MAC[i] = ARP_packet->ARP_sourceMac[i];
		break;
	}
}
