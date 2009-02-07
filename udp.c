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
///		Der UDP-Stack f�r Mikrocontroller. Behandelt komplett den UDP-Stack
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
#include <avr/pgmspace.h>
#include "math.h"
#include "arp.h"
#include "ethernet.h"
#include "ip.h"
#include "udp.h"
#include "enc28j60.h"
#include "timer1.h"
#include "checksum.h"

struct UDP_SOCKET sock;

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
void udp (unsigned int packet_length, unsigned char * ethernetbuffer)
{
	
	struct ETH_header * ETH_packet; 		// ETH_struct anlegen
	ETH_packet = (struct ETH_header *) ethernetbuffer;
	struct IP_header * IP_packet;		// IP_struct anlegen
	IP_packet = ( struct IP_header *) &ethernetbuffer[ETHERNET_HEADER_LENGTH];
	struct UDP_header * UDP_packet;		// TCP_struct anlegen
	UDP_packet = ( struct UDP_header *) &ethernetbuffer[ETHERNET_HEADER_LENGTH + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];

	if (sock.SourcePort == UDP_packet->UDP_DestinationPort && sock.Bufferfill == 0)
	{

		// Gr��e der Daten eintragen
		sock.Bufferfill = htons(UDP_packet->UDP_Datalenght) - UDP_HEADER_LENGTH;

		// TFTP: Zielport �ndern auf SourcePort des empfangenen Pakets (TID)
		sock.DestinationPort = UDP_packet->UDP_SourcePort;


		//UDPRxBuffer = (ethernetbuffer + (ETHERNET_HEADER_LENGTH + 20 + UDP_HEADER_LENGTH));

		// Offset f�r UDP-Daten im Ethernetfrane berechnen
		unsigned int Offset, i;
		Offset = ETHERNET_HEADER_LENGTH + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 ) + UDP_HEADER_LENGTH;
		i = sock.Bufferfill;
		
		// Daten kopieren
		while (i--)
		{
			UDPRxBuffer[i] = ethernetbuffer[Offset + i];
		}

		
	}
}

/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Registriert ein Socket in den die Daten fuer ein Verbindung gehalten werden um die ausgehenden und einghenden UDP-Packet zuzuordnen.
 * \param 	IP					Die IP-Adresse des Zielhost.
 * \param	DestinationPort		Der Zielpot des Zielhost mit den verbunden werden soll. Der Sourcport wird automatisch eingestellt. Zu beachten ist das bei einer Verbindn zu Port 67 der Sourceport auf 68 eingestellt wird.
 * \param	Bufferlenght		Groesse des Datenpuffer der vom Benutzer bereitgestellt wird. Hier werden die eingegenden UDP-Daten kopiert. Dieser Puffer sollte entsprechend der Verwendung dimensioniert sein.
 * \param	UDP_Recivebuffer	Zieger auf den Puffer der vom Benutzer bereitgestellt wird.
 * \return  Beim erfolgreichen anlegen eines Socket wird die Socketnummer zurueck gegeben. Im Fehlerfall 0xffff.
 */
/* -----------------------------------------------------------------------------------------------------------*/
void UDP_RegisterSocket (unsigned long IP, unsigned int DestinationPort)
{
	unsigned char i;

	sock.DestinationPort = htons(DestinationPort);
	sock.SourcePort =~ DestinationPort;

	sock.DestinationIP = IP;
	sock.Bufferfill = 0;
	//sock.Recivebuffer = (ethernetbuffer + (ETHERNET_HEADER_LENGTH + 20 + UDP_HEADER_LENGTH));

	if ( IP == 0xffffffff )
	{
		for( i = 0 ; i < 6 ; i++ ) sock.MACadress[i] = 0xff;
		return;
	}
/*
	if (IS_ADDR_IN_MY_SUBNET (IP, mlNetmask))
		if ( IS_BROADCAST_ADDR( IP, mlNetmask ) ) for( i = 0 ; i < 6 ; i++ ) sock.MACadress[i] = 0xff;
		else GetIP2MAC( IP, &sock.MACadress );
	else GetIP2MAC( mlGateway, &sock.MACadress );
*/
	return;
}



/* -----------------------------------------------------------------------------------------------------------*/
/*!\brief Sendet ein UDP-Packet an einen Host.
 * \param 	SOCKET			Die Socketnummer ueber die das Packet gesendet wird.
 * param	Datalenght		Gibt die Datenlaenge der Daten in Byte an die gesendet werden sollen.
 * \param	UDP_Databuffer  Zeifer auf den Datenpuffer der gesendet werden soll.
 * \return  Bei einem Fehler beim versenden wird ungleich 0 zurueckgegeben, sonst 0.
 * \sa UDP_RegisterSocket , UDP_GetSocketState
 */
/* -----------------------------------------------------------------------------------------------------------*/
void UDP_SendPacket (unsigned int datalength)
{

	struct ETH_header * ETH_packet; 		// ETH_struct anlegen
	ETH_packet = (struct ETH_header *) ethernetbuffer_send;
	struct IP_header * IP_packet;		// IP_struct anlegen
	IP_packet = ( struct IP_header *) &ethernetbuffer_send[ETHERNET_HEADER_LENGTH];
	IP_packet->IP_Version_Headerlen = 0x45; // Standard IPv4 und Headerlenghth 20byte
	struct UDP_header * UDP_packet;		// UDP_struct anlegen
	UDP_packet = ( struct UDP_header *) &ethernetbuffer_send[ETHERNET_HEADER_LENGTH + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];

 
        // MakeIPHeader
	IP_packet->IP_Version_Headerlen = 0x45;
	IP_packet->IP_TOS = 0x0;
	IP_packet->IP_Totallenght = htons( IP_HEADER_LENGHT + UDP_HEADER_LENGTH + datalength );
	IP_packet->IP_Identification = 0x1DAC;
	IP_packet->IP_Flags = 0x40;
	IP_packet->IP_Fragmentoffset = 0x0;
	IP_packet->IP_TTL = 64 ;
	IP_packet->IP_Protocol = PROTO_UDP;
	IP_packet->IP_Headerchecksum = 0x0;
	IP_packet->IP_SourceIP = mlIP;
	IP_packet->IP_DestinationIP = sock.DestinationIP;
	IP_packet->IP_Headerchecksum = htons( Checksum_16( &ethernetbuffer_send[ETHERNET_HEADER_LENGTH] ,(IP_packet->IP_Version_Headerlen & 0x0f) * 4 ) );

	// MakeUDPheader
	UDP_packet->UDP_DestinationPort = sock.DestinationPort;
	UDP_packet->UDP_SourcePort = sock.SourcePort;
	UDP_packet->UDP_Checksum = 0;
	UDP_packet->UDP_Datalenght = htons(8 + datalength);

	MakeETHheader ((unsigned char *)sock.MACadress, ethernetbuffer_send);

	// sendEthernetframe
	sendEthernetframe (ETHERNET_HEADER_LENGTH + IP_HEADER_LENGHT
						+ UDP_HEADER_LENGTH + datalength, ethernetbuffer_send);

}


//@}