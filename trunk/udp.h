/*!\file udp.h \brief Definitionen fuer UDP */
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

#ifndef _UDP_H
#define _UDP_H



	typedef struct
	{
		unsigned int port;		// Port
		void(*fp)(void);  	// Zeiger auf auszuführende Funktion
	} UDP_PORT_ITEM;

	/*!\struct UDP_header
	 * \brief Definiert den UDP_header.
	 */
	struct UDP_header {
		unsigned int UDP_SourcePort;
		unsigned int UDP_DestinationPort;
		unsigned int UDP_Datalenght;
		unsigned int UDP_Checksum;
	};

	
	void udp_init(void) BOOTLOADER_SECTION;

	/* -----------------------------------------------------------------------------------------------------------*/
	/*! Hier findet die Bearbeitung des Packetes statt welches ein UDP-Packet enthaelt. Es wir versucht die
	 * Verbindung zuzuordnen, wenn dies nicht moeglich ist wird hir abgebrochen.
	 * Danach wird der Inhalt dem Socket zugeordnet und Daten in den Puffer des Benutzer kopiert.
	 * \warning Zu lange UDP-Packete werden abgeschnitten.
	 * \param 	packet_lenght	Gibt die Packetgroesse in Byte an die das Packet lang ist.
	 * \param	ethernetbuffer	Zeiger auf das Packet.
	 * \return  NONE
	 */
	/* -----------------------------------------------------------------------------------------------------------*/
	void udp (void) BOOTLOADER_SECTION;

	/* -----------------------------------------------------------------------------------------------------------*/
	/*!\brief Sendet ein UDP-Packet an einen Host.
	 * \param 	SOCKET			Die Socketnummer ueber die das Packet gesendet wird.
	 * param	Datalenght		Gibt die Datenlaenge der Daten in Byte an die gesendet werden sollen.
	 * \param	UDP_Databuffer  Zeifer auf den Datenpuffer der gesendet werden soll.
	 * \return  Bei einem Fehler beim versenden wird ungleich 0 zurueckgegeben, sonst 0.
	 * \sa UDP_RegisterSocket , UDP_GetSocketState
	 */
	/* -----------------------------------------------------------------------------------------------------------*/
void UDP_SendPacket(unsigned int data_length, 
					unsigned int src_port, 
					unsigned int dest_port, 
					unsigned long dest_ip)
		BOOTLOADER_SECTION;

uint8_t UDP_RegisterSocket (unsigned int port, void(*fp1)(void)) BOOTLOADER_SECTION;
void UDP_UnRegisterSocket (unsigned int port) BOOTLOADER_SECTION;

	

#endif /* _UDP_H */
