/***************************************************************************
 *            eemem.h
 *
 *  11.10.2007
 *  Copyright  2007  jkrause
 * 
 *  Diese Datei enthält alle Definitionen von EEMEM Variablen, die
 *  im EEPROM gespeichert werden
 ****************************************************************************/

#ifndef __EEMEM_H__
	
	#define __EEMEM_H__

#include <avr/eeprom.h>
#include "config.h"


extern unsigned long EEMEM mlIpEEP;
extern unsigned long EEMEM mlNetmaskEEP;
extern unsigned long EEMEM mlGatewayEEP;
extern unsigned long EEMEM mlDNSserverEEP;
//extern uint8_t EEMEM miDeviceID;
extern unsigned char EEMEM maTFTPReqStr[12];

#if USE_ENC28J60	
extern unsigned char EEMEM enc28j60_config[];
#endif

#endif
