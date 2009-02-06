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

//convert config ip to long:
#ifndef IP
	#define IP(a0,a1,a2,a3) ((((unsigned long)(a3))<<24)|(((unsigned long)(a2))<<16)|(((unsigned long)(a1))<<8)|(unsigned long)(a0))
#endif

extern unsigned char EEMEM maMacEEP[6];
extern unsigned long EEMEM mlIpEEP;
extern unsigned long EEMEM mlNetmaskEEP;
extern unsigned long EEMEM mlGatewayEEP;
extern unsigned long EEMEM mlDNSserverEEP;
//extern uint8_t EEMEM miDeviceID;
extern unsigned char EEMEM maTFTPReqStr[12];

#endif
