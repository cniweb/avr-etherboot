#include "config.h"
#include "arp.h"
#include "ip.h"
#include "ethernet.h"
#include "udp.h"
#include "math.h"
#include "checksum.h"
#include "eemem.h"

unsigned long mlIP;
unsigned long mlNetmask;
unsigned long mlGateway;
unsigned long mlDNSserver;

void ip_init (void)
{

	// Adressen aus dem EEPROM lesen
	eeprom_read_block ((void*)&mlIP, (const void*)&mlIpEEP, 4);
	eeprom_read_block ((void*)&mlNetmask, (const void*)&mlNetmaskEEP, 4);
	eeprom_read_block ((void*)&mlGateway, (const void*)&mlGatewayEEP, 4);
	eeprom_read_block ((void*)&mlDNSserver, (const void*)&mlDNSserverEEP, 4);

}

