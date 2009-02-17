#include "eemem.h"


unsigned long EEMEM mlIpEEP = IP(192,168,2,131);
unsigned long EEMEM mlNetmaskEEP = IP(255,255,0,0);
unsigned long EEMEM mlGatewayEEP = IP(0,0,0,0);
unsigned long EEMEM mlDNSserverEEP = IP(0,0,0,0); //0x0302a8c0;
//uint8_t EEMEM miDeviceID = 001;
unsigned char EEMEM maTFTPReqStr[TFTPReqStrSize] = {0x00, 0x01, 't', 's', 't', '.', 'h', 'e', 'x', 0x00, 'o', 'c', 't', 'e', 't', 0x00};

#if USE_ENC28J60	
#include "enc28j60.h"

//store enc28j60 config in eeprom memory
//syntax: <REGx>, <REGVALx>, <REGy>, <REGVALy>,...
unsigned char EEMEM enc28j60_config[]  = {
	//setup bank0 (config stored in progmem, see above)
	//tx buffer:
	ETXSTL, lo8(ENC28J60_TX_BUFFER_START), //start lo
	ETXSTH, hi8(ENC28J60_TX_BUFFER_START), //start hi
	ETXNDL, lo8(ENC28J60_TX_BUFFER_END  ), //end lo
	ETXNDH, hi8(ENC28J60_TX_BUFFER_END  ), //end hi
	//rx buffer
	ERXSTL, lo8(ENC28J60_RX_BUFFER_START), //start lo
	ERXSTH, hi8(ENC28J60_RX_BUFFER_START), //start hi
	ERXNDL, lo8(ENC28J60_RX_BUFFER_END  ), //end lo
	ERXNDH, hi8(ENC28J60_RX_BUFFER_END  ), //end hi
	//rx ptr:
	//ERXRDPTL, lo8(ENC28J60_RX_BUFFER_START+1),
	//ERXRDPTH, hi8(ENC28J60_RX_BUFFER_START+1),

	//setup bank2: (see microchip datasheet p.36)
	//1.) clear the MARST bit in MACON2.
	MACON2, 	0x00,
	//2.) mac rx enable, activate pause control frame support
	MACON1, 	MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS,
	//3.) setup MACON3: full duplex, auto padding of small packets, add crc, enable frame length check:
	// !!! must change PHCON1 in enc28j60.c for half duplex !!!
	MACON3, 	MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX,
	//4.) don't set up MACON4 (use default)
	//5.) setup maximum framelenght to 1518:
	MAMXFLL, lo8(1518),
	MAMXFLH, hi8(1518),
	//6.) set up back-to-back gap: 0x15 for full duplex / 0x12 for half duplex
	MABBIPG, 0x15, // full duplex, use 0x12 for half duplex
	//7.) setup non-back-to-back gap: use 0x12
	MAIPGL,  0x12,
	//8.) setup non-back-to-back gap high byte: 0x0C for half duplex:
	MAIPGH,  0x00, // full duplex, use 0x0C for half duplex
	//9.) don't change MACLCON1+2 / MACLCON2 might be changed for networks with long wires !

	//setup bank3:
	//10.) programm mac address: BYTE BACKWARD !
	MAADR5, MYMAC1,
	MAADR4, MYMAC2,
	MAADR3, MYMAC3,
	MAADR2, MYMAC4,
	MAADR1, MYMAC5,
	MAADR0, MYMAC6
	};
#endif

