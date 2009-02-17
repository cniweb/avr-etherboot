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
#define TFTPReqStrSize	16
extern unsigned char EEMEM maTFTPReqStr[TFTPReqStrSize];

#if USE_ENC28J60	
extern unsigned char EEMEM enc28j60_config[];
#define ENC28J60_CONFIG_OFFSET_MAC 33
#endif

#endif
