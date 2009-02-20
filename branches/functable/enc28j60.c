/*! \file enc28j60.c \brief Microchip ENC28J60 Ethernet Interface Driver. */
//*****************************************************************************
//
// File Name	: 'enc28j60.c'
// Title		: Microchip ENC28J60 Ethernet Interface Driver
// Author		: Pascal Stang (c)2005
// Created		: 9/22/2005
// Revised		: 9/22/2005
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
// Description	: This driver provides initialization and transmit/receive
//	functions for the Microchip ENC28J60 10Mb Ethernet Controller and PHY.
// This chip is novel in that it is a full MAC+PHY interface all in a 28-pin
// chip, using an SPI interface to the host processor.
//
//*****************************************************************************
#include <avr/io.h>
#include <util/delay.h>
#include "config.h"
#include "eemem.h"
#include "enc28j60.h"
#include "spi.h"
#include "functable.h"
#include <avr/pgmspace.h>

#if USE_ENC28J60

//#include "math.h"
//#include "config.h"

unsigned char Enc28j60Bank;
unsigned int NextPacketPtr;


//*********************************************************************************************************
//
// Sende Read Command
//
//*********************************************************************************************************
 unsigned char enc28j60ReadOp( unsigned char op,  unsigned char address)
{
	unsigned char data;
	// CS aktive setzen
	ENC28J60_CONTROL_PORT &= ~(1<<ENC28J60_PIN_CS);
	// lesecomando schreiben
	data = SPI_ReadWrite( op | (address & ADDR_MASK) );
	// dummy senden um ergebnis zu erhalten
	data = SPI_ReadWrite( 0x00 );
	// dummy read machen
	if ( address & 0x80 )
		data = SPI_ReadWrite( 0x00 );
	// CS wieder freigeben
	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_PIN_CS);
	
	return data;
}

//*********************************************************************************************************
//
// Sende Write Command
//
//*********************************************************************************************************
void enc28j60WriteOp( unsigned char op, unsigned char address,  unsigned char data)
{
	// CS aktive setzen
	ENC28J60_CONTROL_PORT &= ~(1<<ENC28J60_PIN_CS);
	// schreibcomando senden
	SPI_ReadWrite( op | (address & ADDR_MASK) );
	// daten senden
	SPI_ReadWrite( data );
	// CS wieder freigeben
	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_PIN_CS);
}

//*********************************************************************************************************
//
// Buffer einlesen
//
//*********************************************************************************************************
void enc28j60ReadBuffer(unsigned int len, unsigned char * data)
{
#if DEBUG_AV && DEBUG_ENC_BUFFER_DATA
	putpgmstring("enc28j60ReadBuffer start\r\n");
#endif	
	// assert CS
	ENC28J60_CONTROL_PORT &= ~(1<<ENC28J60_PIN_CS);
	
	// issue read command
	SPI_ReadWrite( ENC28J60_READ_BUF_MEM );

	while(len--)
	{
		// read data
		*data++ = SPI_ReadWrite( 0x00 );
#if DEBUG_AV && DEBUG_ENC_BUFFER_DATA
		puthexbyte(*(data-1));
#endif
	}

	// release CS

	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_PIN_CS);
#if DEBUG_AV && DEBUG_ENC_BUFFER_DATA
	putpgmstring("\r\n");
	putpgmstring("enc28j60ReadBuffer end\r\n");
#endif	
}

//*********************************************************************************************************
//
// Buffer schreiben
//
//*********************************************************************************************************
void enc28j60WriteBuffer(unsigned int len, unsigned char * data)
{
#if DEBUG_AV && DEBUG_ENC_BUFFER_DATA
	putpgmstring("enc28j60WriteBuffer start\r\n");
#endif	
	// assert CS
	ENC28J60_CONTROL_PORT &= ~(1<<ENC28J60_PIN_CS);

	// issue write command
	SPI_ReadWrite( ENC28J60_WRITE_BUF_MEM );

	while(len--)
	{
#if DEBUG_AV && DEBUG_ENC_BUFFER_DATA
		puthexbyte(*data);
#endif
		// write data
		SPI_ReadWrite( *data++ );
	}
	// release CS
	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_PIN_CS);
#if DEBUG_AV && DEBUG_ENC_BUFFER_DATA
	putpgmstring("\r\n");
	putpgmstring("enc28j60WriteBuffer end\r\n");
#endif	
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
void enc28j60SetBank(unsigned char address)
{
	// set the bank (if needed)
	if((address & BANK_MASK) != Enc28j60Bank)
	{
		// set the bank
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
		Enc28j60Bank = (address & BANK_MASK);
	}
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
unsigned char enc28j60Read(unsigned char address)
{
	// set the bank
	enc28j60SetBank(address);
	// do the read	
	return enc28j60ReadOp(ENC28J60_READ_CTRL_REG, address);
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
void enc28j60Write(unsigned char address, unsigned char data)
{
	// set the bank
	enc28j60SetBank(address);
	// do the write
	enc28j60WriteOp(ENC28J60_WRITE_CTRL_REG, address, data);
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
unsigned int enc28j60PhyRead(unsigned char address)
{
     unsigned int data;
 
     // Set the right address and start the register read operation
     enc28j60Write(MIREGADR, address);
     enc28j60Write(MICMD, MICMD_MIIRD);
 
    // wait until the PHY read completes
     while(enc28j60Read(MISTAT) & MISTAT_BUSY);
 
     // quit reading
     enc28j60Write(MICMD, 0x00);
     
     // get data value
     data  = enc28j60Read(MIRDL);
     data |= enc28j60Read(MIRDH)<<8;
     // return the data
     return data;
}

//*********************************************************************************************************
//
// 
//
//*********************************************************************************************************
void enc28j60PhyWrite(unsigned char address, unsigned int data)
{
	// set the PHY register address
	enc28j60Write(MIREGADR, address);

	// write the PHY data
	enc28j60Write(MIWRL, data);
	enc28j60Write(MIWRH, data>>8);

	// wait until the PHY write completes
	while(enc28j60Read(MISTAT) & MISTAT_BUSY);
}

//*********************************************************************************************************
//
// Initialiesiert den ENC28J60
//
//*********************************************************************************************************
void enc28j60Init(void)
{
	uint8_t i;
	// initialize I/O
	SPI_init ();	

	ENC28J60_CONTROL_DDR |= (1<<ENC28J60_PIN_CS);
	ENC28J60_CONTROL_PORT |= (1<<ENC28J60_PIN_CS);

	for (i=0;i<30;i++)
		_delay_ms(35);

	// perform system reset
	enc28j60WriteOp(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET);

	for (i=0;i<30;i++)
		_delay_ms(35);

  	// check CLKRDY bit to see if reset is complete
	// while(!(enc28j60Read(ESTAT) & ESTAT_CLKRDY));
    
	// ENC28J60 Rev. B7 Silicon Errata workaround #1, ESTAT.CLKRDY is not working
    // workaround by waiting  1ms
	_delay_ms(1);

	// set receive buffer start address
	NextPacketPtr = ENC28J60_RX_BUFFER_START;
	
	//copy config from eemem to enc28j60:
	for(i=0; i<2*22; i+=2){
		enc28j60Write(eeprom_read_byte(&enc28j60_config[i+0]),eeprom_read_byte(&enc28j60_config[i+1]));
	}

	// enable full duplex (see also MACON3 in eeprom)
	enc28j60PhyWrite(PHCON1, PHCON1_PDPXMD);

	// no loopback of transmitted frames
	enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);

	// switch to bank 0
	enc28j60SetBank(ECON1);

	// enable interrutps
//	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE);
	// disable interrupts
	enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIE, EIE_INTIE);
	// enable packet reception
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
	//set up leds: LEDA: link status, LEDB: RX&TX activity, stretch 40ms, stretch enable
	enc28j60PhyWrite(PHLCON, 0x347A); //cave: Table3-3: reset value is 0x3422, do not modify the reserved "3"!! 
													 //RevA Datasheet page 9: write as '0000', see RevB Datasheet: write 0011!

}

//*********************************************************************************************************
//
// Sendet ein Packet
//
//*********************************************************************************************************
void enc28j60PacketSend(unsigned int len, unsigned char* packet)
{

	// ENC28J60 Rev. B7 Silicon Errata workaround #10,
	// Internal transmit logic may stall in half duplex mode
	// on excessive collisions, a late collision or excessive deferrals
	// In this case the Transmit Error Interrupt Flag is set. Check it
	if( !(enc28j60Read(EIR) & EIR_TXERIF) )
	{	// Reset transmit logic
		enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
		enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXERIF);
	}	
	
	// Set the write pointer to start of transmit buffer area
	enc28j60Write(EWRPTL, (uint8_t)ENC28J60_TX_BUFFER_START);
	enc28j60Write(EWRPTH, ENC28J60_TX_BUFFER_START>>8);
	
	// Set the TXND pointer to correspond to the packet size given
	enc28j60Write(ETXNDL, (ENC28J60_TX_BUFFER_START+len));
	enc28j60Write(ETXNDH, (ENC28J60_TX_BUFFER_START+len)>>8);
 	 
	// write per-packet control byte
	enc28j60WriteOp(ENC28J60_WRITE_BUF_MEM, 0, 0x00);

	// copy the packet into the transmit buffer
	enc28j60WriteBuffer(len, packet);
	
	// clear transmit interrupt flag
	enc28j60WriteOp(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXIF);
		
	// send the contents of the transmit buffer onto the network
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);

}

//*********************************************************************************************************
//
// Hole ein empfangenes Packet
//
//*********************************************************************************************************

unsigned int enc28j60PacketReceiveLenght( void )
{
	unsigned int len;

	enc28j60Write(ERDPTL, (NextPacketPtr));
	enc28j60Write(ERDPTH, (NextPacketPtr)>>8);

	// read the packet length
	len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	
	return( len );
}


//*********************************************************************************************************
//
// Hole ein empfangendes Packet
//
//*********************************************************************************************************
unsigned int enc28j60PacketReceive(unsigned int maxlen, unsigned char* packet)
{
	unsigned int rxstat;	//, rs,re;
	unsigned int len;
	uint8_t nPktCnt = enc28j60Read(EPKTCNT);

	// check if a packet has been received and buffered
	if( !(enc28j60Read(EIR) & EIR_PKTIF) )
	{
		// ENC28J60 Rev. B7 Silicon Errata workaround #4, PKTIF is not reliable
        // double check by looking at EPKTCNT
		if (nPktCnt == 0)
		{
			return 0;
		}
	}

	// Set the read pointer to the start of the received packet
	enc28j60Write(ERDPTL, (NextPacketPtr));
	enc28j60Write(ERDPTH, (NextPacketPtr)>>8);
	 	
	
	// read the next packet pointer
	NextPacketPtr  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	NextPacketPtr |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	
	// read the packet length
	len  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	len |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	
	// read the receive status
	rxstat  = enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= enc28j60ReadOp(ENC28J60_READ_BUF_MEM, 0)<<8;
	
	// limit retrieve length
	// When len bigger than maxlen, ignore the packet und read next packetptr
	if (len <= maxlen)
	{
		// buffer is big enough
		// copy the packet from the receive buffer
		enc28j60ReadBuffer(len, packet);
	}
	
	// Move the RX read pointer to the start of the next received packet
	// This frees the memory we just read out
	// ENC28J60 Rev. B7 Silicon Errata workaround #11, receive buffer may get corrupted
	// when ERXRDPTH:ERXRDPTL contains an even address
/* no need to read the stuff from the chip, we never change these values while running
    rs = enc28j60Read(ERXSTH);
    rs <<= 8;
    rs |= enc28j60Read(ERXSTL);
    re = enc28j60Read(ERXNDH);
    re <<= 8;
    re |= enc28j60Read(ERXNDL);
    if (NextPacketPtr - 1 < rs || NextPacketPtr - 1 > re)
*/
	// use defined values instead
	if (((NextPacketPtr - 1) < ENC28J60_RX_BUFFER_START) || ((NextPacketPtr - 1) > ENC28J60_RX_BUFFER_END))
	{
        enc28j60Write(ERXRDPTL, lo8(ENC28J60_RX_BUFFER_END));
        enc28j60Write(ERXRDPTH, hi8(ENC28J60_RX_BUFFER_END));
    }
    else
    {
        enc28j60Write(ERXRDPTL, (NextPacketPtr-1));
        enc28j60Write(ERXRDPTH, (NextPacketPtr-1)>>8);
    }
	
	// decrement the packet counter indicate we are done with this packet
	enc28j60WriteOp(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
	
	// remove this part in half duplex mode!
	if (nPktCnt > 2)
	{	// at least two more packets are waiting, we need to slow down the sender
		enc28j60Write(EFLOCON, 0x02);	// Send pause frames periodically
	}
	else if (nPktCnt == 1)
	{	// buffer is empty now
		if (enc28j60Read(EFLOCON) & 0x03)
		{	// FCEN1 or FCEN0 is set, tell sender to continue now
			enc28j60Write(EFLOCON, 0x03);	// Send one pause frame with a 0 timer value and then turn flow control off
		}
	}

	return (maxlen >= len ? len : 0);
	
}
#endif
