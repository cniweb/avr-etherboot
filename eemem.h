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


extern unsigned long EEMEM mlIpEEP;
extern unsigned long EEMEM mlNetmaskEEP;
extern unsigned long EEMEM mlGatewayEEP;
extern unsigned long EEMEM mlDNSserverEEP;

typedef struct 
{
	unsigned short opcode;
	unsigned char fnandmode[];
} TFTPREQ;
extern TFTPREQ maTFTPReqStr EEMEM;
#define TFTPReqStrSize 		16		// stupid compiler does not want to calculate 
									// sizeof(maTFTPReqStr), has to be done by hand 

typedef struct 
{
	unsigned short opcode;
	unsigned short errcode;
	unsigned char errstring[];
} TFTPERR;
extern TFTPERR maTFTPErrStr EEMEM;
#define TFTPErrStrSize 		34		// stupid compiler does not want to calculate 
									// sizeof(maTFTPErrStr), has to be done by hand 


#if USE_ENC28J60	
extern unsigned char EEMEM enc28j60_config[];
#define ENC28J60_CONFIG_OFFSET_MAC 33
#endif

#endif
