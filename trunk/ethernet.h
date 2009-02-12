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

#define ETHERNET_MIN_PACKET_LENGTH	0x3C
 
//IP Protocol Types
#define	PROT_ICMP				0x01	//zeigt an die Nutzlasten enthalten das ICMP Prot
#define	PROT_TCP				0x06	//zeigt an die Nutzlasten enthalten das TCP Prot.
#define	PROT_UDP				0x11	//zeigt an die Nutzlasten enthalten das UDP Prot.	

#define IP_HDR_LEN 				20
#define TCP_HDR_LEN 			20
#define UDP_HDR_LEN				8
#define ETH_HDR_LEN 			14
#define TCP_DATA_START			(IP_VERS_LEN + TCP_HDR_LEN + ETH_HDR_LEN)
#define UDP_DATA_START			(IP_VERS_LEN + UDP_HDR_LEN + ETH_HDR_LEN)

struct ETH_header {
	unsigned char ETH_destMac[6];	
	unsigned char ETH_sourceMac[6];
	unsigned int ETH_typefield;
};

//----------------------------------------------------------------------------
//Aufbau eines IP Datagramms (B=BIT)
//	
//4B Version	|4B Headergr.	|8B Tos	|16B Gesamtlänge in Bytes	
//16B Identifikation			|3B Schalter	|13B Fragmentierungsposition
//8B Time to Live	|8B Protokoll	|16B Header Prüfsumme 
//32B IP Quelladresse
//32B IB Zieladresse
struct IP_Header	{
	unsigned char	IP_Version_Headerlen;	//4 BIT Die Versionsnummer von IP, 
											//meistens also 4 + 4Bit Headergröße 	
	unsigned char	IP_Tos;					//Type of Service
	unsigned int	IP_Totallenght;			//16 Bit Komplette Läng des IP Datagrams in Bytes
	unsigned int	IP_Id;					//ID des Packet für Fragmentierung oder 
											//Reassemblierung
	unsigned char	IP_Flags;
	unsigned int	IP_Frag_Offset;			//wird benutzt um ein fragmentiertes 
											//IP Packet wieder korrekt zusammenzusetzen
	unsigned char	IP_ttl;					//8 Bit Time to Live die lebenszeit eines Paket
	unsigned char	IP_Proto;				//Zeigt das höherschichtige Protokoll an 
											//(TCP, UDP, ICMP)
	unsigned int	IP_Hdr_Cksum;			//Checksumme des IP Headers
	unsigned long	IP_Srcaddr;				//32 Bit IP Quelladresse
	unsigned long	IP_Destaddr;			//32 Bit IP Zieladresse
};



extern unsigned long mlIP;
extern unsigned long mlNetmask;
extern unsigned long mlGateway;
extern unsigned long mlDNSserver;
extern unsigned char ethernetbuffer[];

void stack_init (void);

void eth_packet_dispatcher(void);

void MakeETHheader (unsigned char * MACadress);


#endif
