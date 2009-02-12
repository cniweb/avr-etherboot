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

	#define htons(A) ((((A) & 0xff00) >> 8) | (((A) & 0x00ff) << 8))
	#define htonl(A) ((((A) & 0xff000000) >> 24) | (((A) & 0x00ff0000) >> 8) | \
                 (((A) & 0x0000ff00) << 8) | (((A) & 0x000000ff) << 24)) 
	#define ntohs htons 
	#define ntohl htonl


	#define _UDP_H

    extern struct UDP_SOCKET sock;

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
	void udp (unsigned int packet_length);

	/* -----------------------------------------------------------------------------------------------------------*/
	/*!\brief Sendet ein UDP-Packet an einen Host.
	 * \param 	SOCKET			Die Socketnummer ueber die das Packet gesendet wird.
	 * param	Datalenght		Gibt die Datenlaenge der Daten in Byte an die gesendet werden sollen.
	 * \param	UDP_Databuffer  Zeifer auf den Datenpuffer der gesendet werden soll.
	 * \return  Bei einem Fehler beim versenden wird ungleich 0 zurueckgegeben, sonst 0.
	 * \sa UDP_RegisterSocket , UDP_GetSocketState
	 */
	/* -----------------------------------------------------------------------------------------------------------*/
	void UDP_SendPacket (unsigned int datalength);

	/* -----------------------------------------------------------------------------------------------------------*/
	/*!\brief Reistriert ein Socket in den die Daten fuer ein Verbindung gehalten werden um die ausgehenden und einghenden UDP-Packet zuzuordnen.
	 * \param 	IP					Die IP-Adresse des Zielhost.
	 * \param	DestinationPort		Der Zielpot des Zielhost mit den verbunden werden soll. Der Sourcport wird automatisch eingestellt. Zu beachten ist das bei einer Verbindn zu Port 67 der Sourceport auf 68 eingestellt wird.
	 * \param	Bufferlenght		Groesse des Datenpuffer der vom Benutzer bereitgestellt wird. Hier werden die eingegenden UDP-Daten kopiert. Dieser Puffer sollte entsprechend der Verwendung dimensioniert sein.
	 * \param	UDP_Recivebuffer	Zieger auf den Puffer der vom Benutzer bereitgestellt wird.
	 * \return  Beim erfolgreichen anlegen eines Socket wird die Socketnummer zurueck gegeben. Im Fehlerfall 0xffff.
	 */
	/* -----------------------------------------------------------------------------------------------------------*/
	void UDP_RegisterSocket (unsigned long IP, unsigned int DestinationPort);

	/* -----------------------------------------------------------------------------------------------------------*/
	/*!\brief Reistriert ein Socket in den die Daten fuer ein Verbindung gehalten werden um die ausgehenden und einghenden UDP-Packet zuzuordnen.
	 * \param 	IP					Die IP-Adresse des Zielhost.
	 * \param	DestinationPort		Der Zielpot des Zielhost mit den verbunden werden soll. Der Sourcport wird automatisch eingestellt. Zu beachten ist das bei einer Verbindn zu Port 67 der Sourceport auf 68 eingestellt wird.
	 * \param	Bufferlenght		Groesse des Datenpuffer der vom Benutzer bereitgestellt wird. Hier werden die eingegenden UDP-Daten kopiert. Dieser Puffer sollte entsprechend der Verwendung dimensioniert sein.
	 * \param	UDP_Recivebuffer	Zieger auf den Puffer der vom Benutzer bereitgestellt wird.
	 * \return  Beim erfolgreichen anlegen eines Socket wird die Socketnummer zurueck gegeben. Im Fehlerfall 0xffff.
	 */
	/* -----------------------------------------------------------------------------------------------------------*/

	/* -----------------------------------------------------------------------------------------------------------*/
	/*!\brief Gibt den Socketstatus aus.
	 * \param	SOCKET	Die Socketnummer vom abzufragen Socket.
	 * \return  Den Socketstatus.
	 */
	/* -----------------------------------------------------------------------------------------------------------*/
	unsigned int UDP_GetSocketState (void);

	/* -----------------------------------------------------------------------------------------------------------*/
	/*!\brief Gibt die Anzahl der Byte aus die sich im Puffer befinden. Diese Abfrage macht nur sinn in Verbindung mit
	 * UDP_GetSocketState nachdem ein UDP-Packet empfangen worden ist und der Status fuer das auf SOCKET_BUSY steht.
	 * Danach werden bis zur Freigabe durch UDP_FreeBuffer keine Daten auf den Socket mehr angenommen
	 * \param	SOCKET		Die Socketnummer vom abzufragen Socket.
	 * \return  Anzahl der Byte im Puffer.
	 *\sa UDP_GetSocketState, UDP_FreeBuffer
	 */
	/* -----------------------------------------------------------------------------------------------------------*/
	unsigned int UDP_GetByteInBuffer (void);

	/* -----------------------------------------------------------------------------------------------------------*/
	/*!\brief Gibt den UDP-Puffer wieder zum empfang frei. Danach werden wieder UDP-Daten angenommen und in den Puffer kopiert.
	 * \param	SOCKET		Die Socketnummer die freigegeben werden soll.
	 * \return	NONE
	 */
	/* -----------------------------------------------------------------------------------------------------------*/
	 void UDP_FreeBuffer (void);
	
	/* -----------------------------------------------------------------------------------------------------------*/
	/*!\brief Gibt das Socket wieder freu und beendet die Verbindung. Alle UDP-Packet die dann von diesen Socket empfangen werden, werden verworfen.
	 * \param	SOCKET		Die Socketnummer die geschlossen werden soll.
	 * \return	Es wird beim erfolgreichen schliessen der Socket 0 zurueck gegeben, sonst 0xffff.
	 */
	/* -----------------------------------------------------------------------------------------------------------*/

	unsigned int UDP_GetSocket (unsigned char * ethernetbuffer);
	void MakeUDPheader  (unsigned int Datalength, unsigned char * ethernetbuffer);

	#define SOCKET_NOT_USE		0x00			// SOCKET ist Frei
	#define SOCKET_READY		0x10			// Socket ist Bereit zur Benutzung
	#define UDP_SOCKET_BUSY			0x20			// SOCKET belegt ...
	
	/*!\struct UDP_header
	 * \brief Definiert den UDP_header.
	 */
	struct UDP_header {
		volatile unsigned int UDP_SourcePort;
		volatile unsigned int UDP_DestinationPort;
		volatile unsigned int UDP_Datalenght;
		volatile unsigned int UDP_Checksum;
	};
	
	/*! \struct UDP_SOCKET
	 * \brief Definiert den UDP_SOCKET Aufbau. Hier sind alle wichtigen Infomationen enthalten um die Verbindung
	 * zum halten und zuzuordnen
	 */
	struct UDP_SOCKET {
		volatile unsigned char Socketstate;
		volatile unsigned long DestinationIP;
		volatile unsigned int SourcePort;
		volatile unsigned int DestinationPort;
		volatile unsigned char MACadress[6];
		volatile unsigned int Bufferlenght;
		volatile unsigned int Bufferfill;
		volatile unsigned int DataStartOffset;
	};
	

#endif /* _UDP_H */
