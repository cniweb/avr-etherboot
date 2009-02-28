/***************************************************************************
 *            ethernet.h
 *
 *  Sat Jun  3 14:57:38 2006
 *  Copyright  2006  User
 *  Email
 ****************************************************************************/

#ifndef __ETHERNET_H__
#define __ETHERNET_H__


#define POLLINGMODE		0
#define INTERRUPTMODE	1
 
#define DHCP		0x01
#define NTP			0x02
#define IRQMODE     0x04
#define POLLMODE 	0x00
 
// berechnet die Broadcast-Adresse bei gegebener IP-Adresse und Netzmaske
#define CALC_BROADCAST_ADDR( ip, mask ) ( ip | ~mask )

// Testet, ob eine Adresse die Broadcast-Adresse ist (zu einer Netzwerkmaske)
#define IS_BROADCAST_ADDR( ip, mask ) ( ( ip & ~mask ) == ~mask)

// Schaut ob Ziel-IP in diesen Subnet liegt 
#define IS_ADDR_IN_MY_SUBNET( ip, mask ) ( ( ip & ~mask ) == ( mlIP & ~mask ) )

#define htons(A) ((((A) & 0xff00) >> 8) | (((A) & 0x00ff) << 8))
#define htonl(A) ((((A) & 0xff000000) >> 24) | (((A) & 0x00ff0000) >> 8) | \
			 (((A) & 0x0000ff00) << 8) | (((A) & 0x000000ff) << 24)) 
#define ntohs htons 
#define ntohl htonl


#define ETHERNET_MIN_PACKET_LENGTH	0x3C
 
//IP Protocol Types
#define	PROT_ICMP				0x01	//zeigt an die Nutzlasten enthalten das ICMP Prot
#define	PROT_TCP				0x06	//zeigt an die Nutzlasten enthalten das TCP Prot.
#define	PROT_UDP				0x11	//zeigt an die Nutzlasten enthalten das UDP Prot.	

#define ETH_HDR_LEN 			0x0e	// = 14
#define ARP_HDR_LEN 			0x1c	// = 28
#define IP_HDR_LEN 				0x14	// = 20
#define TCP_HDR_LEN 			0x14	// = 20
#define UDP_HDR_LEN				0x08	// = 8
#define TCP_DATA_START			(ETH_HDR_LEN + IP_HDR_LEN + TCP_HDR_LEN)
#define UDP_DATA_START			(ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN)

//#define ETHER_OFFSET			0x00
//#define ARP_OFFSET				0x0E
//#define IP_OFFSET				0x0E
//#define UDP_OFFSET				0x22

#define ARP_REPLY_LEN			60
#define ARP_REQUEST_LEN			42
#define ARP_MAX_ENTRY_TIME 		100 //100sec.

#define MAX_UDP_ENTRY			5		// max possible number of udp connections at a time
#define MAX_ARP_ENTRY			5
	
#define TFTP_SERVER_PORT		69

struct ETH_header {
	unsigned char ETH_destMac[6];	
	unsigned char ETH_sourceMac[6];
	unsigned int ETH_typefield;		//Nutzlast 0x0800=IP Datagramm;0x0806 = ARP
};

//----------------------------------------------------------------------------
//Aufbau eines ARP Header
//	
//	2 BYTE Hardware Typ					|	2 BYTE Protokoll Typ	
//	1 BYTE Länge Hardwareadresse (MAC)	|	1 BYTE Länge Protokolladresse (IP)
//	2 BYTE Operation
//	6 BYTE MAC Adresse Absender			|	4 BYTE IP Adresse Absender
//	6 BYTE MAC Adresse Empfänger		|	4 BYTE IP Adresse Empfänger	
struct ARP_header {
	unsigned int HWtype;				// 2 Byte
	unsigned int Protocoltype;			// 2 Byte
	unsigned char HWsize;				// 1 Byte
	unsigned char Protocolsize;			// 1 Byte
	unsigned int ARP_Opcode;			// 2 Byte
	unsigned char ARP_sourceMac[6];		// 6 Byte
	unsigned long ARP_sourceIP;			// 4 Byte
	unsigned char ARP_destMac[6];		// 6 Byte
	unsigned long ARP_destIP;			// 4 Byte = 28
};

//----------------------------------------------------------------------------
//Aufbau eines IP Datagramms (B=BIT)
//	
//4B Version	|4B Headergr.	|8B Tos	|16B Gesamtlänge in Bytes	
//16B Identifikation			|3B Schalter	|13B Fragmentierungsposition
//8B Time to Live	|8B Protokoll	|16B Header Prüfsumme 
//32B IP Quelladresse
//32B IB Zieladresse
struct IP_header	{
	unsigned char	IP_Version_Headerlen;	//4 BIT Die Versionsnummer von IP, 
											//meistens also 4 + 4Bit Headergröße 	
	unsigned char	IP_Tos;					//Type of Service
	unsigned int	IP_Totallenght;			//16 Bit Komplette Läng des IP Datagrams in Bytes
	unsigned int	IP_Id;					//ID des Packet für Fragmentierung oder 
											//Reassemblierung
	unsigned char	IP_Flags;
	unsigned char	IP_Frag_Offset;			//wird benutzt um ein fragmentiertes 
											//IP Packet wieder korrekt zusammenzusetzen
	unsigned char	IP_ttl;					//8 Bit Time to Live die lebenszeit eines Paket
	unsigned char	IP_Proto;				//Zeigt das höherschichtige Protokoll an 
											//(TCP, UDP, ICMP)
	unsigned int	IP_Hdr_Cksum;			//Checksumme des IP Headers
	unsigned long	IP_Srcaddr;				//32 Bit IP Quelladresse
	unsigned long	IP_Destaddr;			//32 Bit IP Zieladresse
};


#define TFTP_FILENAME_SIZE 20

extern unsigned long mlIP;
extern unsigned long mlNetmask;
extern unsigned long mlGateway;
extern unsigned long mlDNSserver;
extern unsigned char mlMAC[];
extern unsigned char ethernetbuffer[];
extern unsigned long mlTFTPip;
extern unsigned char msTFTPfileName[TFTP_FILENAME_SIZE];

void stack_init (void) BOOTLOADER_SECTION;

void eth_packet_dispatcher(void) BOOTLOADER_SECTION;

void Make_ETH_Header (unsigned char *buffer, unsigned long dest_ip) BOOTLOADER_SECTION;

void Make_IP_Header (unsigned char *buffer, unsigned long dest_ip) BOOTLOADER_SECTION;


#endif
