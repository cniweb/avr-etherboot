/***************************************************************************
 *            ethernet.c
 *
 *  Sat Jun  3 17:25:42 2006
 *  Copyright  2006  User
 *  Email
 ****************************************************************************/
#include <avr/interrupt.h>
#include <stdio.h>
#include "config.h"
#include "ethernet.h"
#include "checksum.h"
#include "arp.h"
#include "enc28j60.h"
#include "udp.h"
#include "eemem.h"
#if USE_DHCP
#include "dhcpc.h"
#endif //USE_DHCP


unsigned long mlIP;
unsigned long mlNetmask;
unsigned long mlGateway;
unsigned long mlDNSserver;
unsigned char mlMAC[6];
unsigned char ethernetbuffer[MTU_SIZE];

unsigned int IP_id_counter;

void stack_init (void)
{
	IP_id_counter = 0;
	
	// Read IP Setup from EEPROM
	mlIP = eeprom_read_dword (&mlIpEEP);
	mlNetmask = eeprom_read_dword (&mlNetmaskEEP);
	mlGateway = eeprom_read_dword (&mlGatewayEEP);
	mlDNSserver = eeprom_read_dword (&mlDNSserverEEP);

	uint8_t i;
	for ( i = 0 ; i < 6 ; i++ )
	{
		mlMAC[i] = eeprom_read_byte(&enc28j60_config[ENC28J60_CONFIG_OFFSET_MAC + i*2]);
	}
	
	udp_init();
	
#if USE_DHCP
	dhcp_init();
#endif //USE_DHCP

}

/*
 -----------------------------------------------------------------------------------------------------------
   Die Routine die die pakete nacheinander abarbeitet
------------------------------------------------------------------------------------------------------------*/


void eth_packet_dispatcher(void)
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
				arp_reply ();
				break;

			case 0x0008:
				if (((struct IP_header *)&ethernetbuffer[ETH_HDR_LEN])->IP_Proto == PROT_UDP)
				{
					udp ();
				}
				break;
		}

	}
}



//----------------------------------------------------------------------------
//Diese Routine Erzeugt ein neuen Ethernetheader
void Make_ETH_Header (unsigned char *buffer, unsigned long dest_ip)
{
	struct ETH_header *ETH_packet; 		// ETH_struct anlegen
	ETH_packet = (struct ETH_header *) ethernetbuffer;

	uint8_t i;
	unsigned char *pDestMac = arp_entry_search(dest_ip);
	for ( i = 0 ; i < 6 ; i++ )
	{
		ETH_packet->ETH_sourceMac[i] = mlMAC[i];
		ETH_packet->ETH_destMac[i] = pDestMac?pDestMac[i]:0xff;
	}

}


//----------------------------------------------------------------------------
//Diese Routine erzeugt ein IP Packet
void Make_IP_Header (unsigned char *buffer, unsigned long dest_ip)
{
    unsigned int result16;  //Checksum
    struct ETH_header *ethernet;
    struct IP_header       *ip;

    ethernet = (struct ETH_header *)&buffer[ETHER_OFFSET];
    ip       = (struct IP_header *)&buffer[IP_OFFSET];

    Make_ETH_Header (buffer, dest_ip);         //Erzeugt einen neuen Ethernetheader
    ethernet->ETH_typefield = 0x0008;	// htons(0x0800) Nutzlast 0x0800=IP

    IP_id_counter++;

    ip->IP_Flags		= 0x40;
    ip->IP_Frag_Offset	= 0x00;  //don't fragment
    ip->IP_ttl			= 64;      //max. hops
    ip->IP_Id			= htons(IP_id_counter);
    ip->IP_Version_Headerlen    = 0x45;  //4 BIT Die Versionsnummer von IP, 
    ip->IP_Tos			= 0;
    ip->IP_Destaddr     = dest_ip;
    ip->IP_Srcaddr		= mlIP;
    ip->IP_Hdr_Cksum	= 0;
  
    //Berechnung der IP Header länge  
    result16 = (ip->IP_Version_Headerlen & 0x0F) << 2;

    //jetzt wird die Checksumme berechnet
    ip->IP_Hdr_Cksum = Checksum_16 (&ip->IP_Version_Headerlen, result16, 0);

    return;
}
