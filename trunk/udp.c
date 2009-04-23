/*!\file udp.c \brief Stellt die UDP Funktionalitaet bereit*/
//***************************************************************************
//*            udp.c
//*
//*  Mon Jul 31 21:46:47 2006
//*  Copyright  2006  User
//*  Email
///	\ingroup network
///	\defgroup UDP Der UDP Stack fuer Mikrocontroller (udp.c)
///	\code #include "arp.h" \endcode
///	\code #include "ethernet.h" \endcode
///	\code #include "ip.h" \endcode
///	\code #include "udp.h" \endcode
///	\par Uebersicht
///		Der UDP-Stack für Mikrocontroller. Behandelt komplett den UDP-Stack
/// mit Verbindungsaufbau und Abbau.
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
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "checksum.h"
#include "ethernet.h"
#include "udp.h"
#include "arp.h"
#include "enc28j60.h"

#if BOOTLOADER_FLAVOR == BL_VERSION_SMALL
  UDP_PORT_ITEM UDP_PORT_TABLE; // Single entry Port-Tabelle
#else
  UDP_PORT_ITEM UDP_PORT_TABLE[MAX_UDP_ENTRY]; // Port-Tabelle
#endif

void udp_init(void)
{

#if BOOTLOADER_FLAVOR == BL_VERSION_SMALL
	UDP_PORT_TABLE.port = 0;
#else
	for (uint8_t i=0; i<MAX_UDP_ENTRY; i++)
		UDP_PORT_TABLE[i].port = 0;
#endif
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Hier findet die Bearbeitung des Packetes statt welches ein UDP-Packet enthaelt. Es wir versucht die 
 * Verbindung zuzuordnen, wenn dies nicht moeglich ist wird hier abgebrochen.
 * Danach wird der Inhalt dem Socket zugeordnet und Daten in den Puffer des Benutzer kopiert.
 * \warning Zu lange UDP-Packete werden abgeschnitten.
 * \param 	packet_length	Gibt die Packetgroesse in Byte an die das Packet lang ist.
 * \param	ethernetbuffer	Zeiger auf das Packet.
 * \return  NONE
 */
/* -----------------------------------------------------------------------------------------------------------*/
void udp (void)
{
	struct IP_header * IP_packet;		// IP_struct anlegen
	IP_packet = ( struct IP_header *) &ethernetbuffer[ETH_HDR_LEN];
	struct UDP_header * UDP_packet;
	UDP_packet = ( struct UDP_header *) &ethernetbuffer[ETH_HDR_LEN + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];

	//UDP DestPort mit Portanwendungsliste durchführen
#if BOOTLOADER_FLAVOR == BL_VERSION_SMALL
	if (UDP_PORT_TABLE.port == htons(UDP_packet->UDP_DestinationPort))
	{
		UDP_PORT_TABLE.fp();
	}
#if DEBUG_AV
	else
	{
		putpgmstring("No funktion for this packet found - discarded\r\n");
	}
#endif	
#else

	unsigned char port_index = 0;	
	
	while (UDP_PORT_TABLE[port_index].port && UDP_PORT_TABLE[port_index].port!=htons(UDP_packet->UDP_DestinationPort))
	{ 
		port_index++;
	}
	
	// Wenn index zu gross, dann beenden keine vorhandene Anwendung für den Port
	if (!UDP_PORT_TABLE[port_index].port)
	{ 
#if DEBUG_AV
		putpgmstring("No funktion for this packet found - discarded\r\n");
#endif	
		return;
	}

	//zugehörige Anwendung ausführen
	UDP_PORT_TABLE[port_index].fp(); 
	return;
#endif
}


//----------------------------------------------------------------------------
//Trägt UDP PORT/Anwendung in Anwendungsliste ein
uint8_t UDP_RegisterSocket (unsigned int port, void(*fp1)(void))
{

#if BOOTLOADER_FLAVOR == BL_VERSION_SMALL
	if (UDP_PORT_TABLE.port != 0)
		return 0;
	else
	{
		UDP_PORT_TABLE.port = port;
		UDP_PORT_TABLE.fp = *fp1;
		return 1;
	}
#else

	unsigned char port_index = 0;
	//Freien Eintrag in der Anwendungliste suchen
	while ((UDP_PORT_TABLE[port_index].port) && (port_index < MAX_UDP_ENTRY))
	{ 
		port_index++;
	}
	if (port_index >= MAX_UDP_ENTRY)
	{
//		DEBUG("Zuviele UDP Anwendungen wurden gestartet\r\n");
		return 0;
	}
//	DEBUG("UDP Anwendung wird in Liste eingetragen: Eintrag %i\r\n",port_index);
	UDP_PORT_TABLE[port_index].port = port;
	UDP_PORT_TABLE[port_index].fp = *fp1;
	return 1;
#endif
}


//----------------------------------------------------------------------------
//Löscht UDP Anwendung aus der Anwendungsliste
void UDP_UnRegisterSocket (unsigned int port)
{
 
#if BOOTLOADER_FLAVOR == BL_VERSION_SMALL
	UDP_PORT_TABLE.port = 0;
#else
	unsigned char i;

    for (i = 0; i < MAX_UDP_ENTRY; i++)
    {
        if ( UDP_PORT_TABLE[i].port == port )
        {
            UDP_PORT_TABLE[i].port = 0;
        }
    }
#endif
}


//----------------------------------------------------------------------------
//Diese Routine Erzeugt ein neues UDP Packet
void UDP_SendPacket(unsigned int  data_length,
					unsigned int  src_port,
					unsigned int  dest_port,
					unsigned long dest_ip)
{
    uint16_t  result16;
    unsigned long result32;

    struct UDP_header *udp;
    struct IP_header  *ip;

    udp = (struct UDP_header *)&ethernetbuffer[ETH_HDR_LEN + IP_HDR_LEN];
    ip  = (struct IP_header  *)&ethernetbuffer[ETH_HDR_LEN];
  
    udp->UDP_SourcePort  = htons(src_port);
    udp->UDP_DestinationPort = htons(dest_port);

    data_length     += UDP_HDR_LEN;                //UDP Packetlength
    udp->UDP_Datalenght = htons(data_length);

    data_length     += IP_HDR_LEN;                //IP Headerlänge
    ip->IP_Totallenght = htons(data_length);
    data_length += ETH_HDR_LEN;
    ip->IP_Proto = PROT_UDP;
    Make_IP_Header (ethernetbuffer,dest_ip);

    udp->UDP_Checksum = 0;
  
    //Berechnet Headerlänge und Addiert Pseudoheaderlänge 2XIP = 8
    result16 = htons(ip->IP_Totallenght) + 8;
    result16 = result16 - ((ip->IP_Version_Headerlen & 0x0F) << 2);
	// will never exceed 0xffff, so word arithmetic should do
    result32 = result16 + 0x09;
  
    //Routine berechnet die Checksumme
    result16 = Checksum_16 ((&ip->IP_Version_Headerlen+12), result16, result32);
    udp->UDP_Checksum = result16;

    ETH_PACKET_SEND(data_length,ethernetbuffer); //send...
#if DEBUG_AV
	putpgmstring("UDP_SendPacket\r\n");
#endif	
    return;
}


//@}
