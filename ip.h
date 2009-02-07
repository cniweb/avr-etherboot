#ifndef __IP_H__

#define __IP_H__


// berechnet die Broadcast-Adresse bei gegebener IP-Adresse und Netzmaske
#define CALC_BROADCAST_ADDR( ip, mask ) ( ip | ~mask )

// Testet, ob eine Adresse die Broadcast-Adresse is (zu einer Netzwerkmaske)
#define IS_BROADCAST_ADDR( ip, mask ) ( ( ip & ~mask ) == ~mask)

// Schaut ob Ziel-IP in diesen Subnet liegt 
#define IS_ADDR_IN_MY_SUBNET( ip, mask ) ( ( ip & ~mask ) == ( mlIP & ~mask ) )

extern unsigned long mlIP;
extern unsigned long mlNetmask;
extern unsigned long mlGateway;
extern unsigned long mlDNSserver;

void ip_init (void);
void ip (unsigned int packet_length , unsigned char *buffer);

#define IP_HEADER_LENGHT 20

union IP_ADDRESS {
	unsigned char IPbyte[4];
	unsigned long IP;
};

struct IP_header{
	unsigned char IP_Version_Headerlen;
	unsigned char IP_TOS;
	unsigned int IP_Totallenght;
	unsigned int IP_Identification;
	unsigned char IP_Flags;
	unsigned char IP_Fragmentoffset;
	unsigned char IP_TTL;
	unsigned char IP_Protocol;
	unsigned int IP_Headerchecksum;
	unsigned long IP_SourceIP;
	unsigned long IP_DestinationIP;
};

#define PROTO_ICMP 0x01
#define PROTO_TCP 0x06
#define PROTO_UDP 0x11


#endif
