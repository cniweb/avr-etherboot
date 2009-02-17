/*! \file "etherflash.c" \brief main bootloader routine  */
//***************************************************************************
//*            etherflash.c
//*
//*  Sun Feb  8 2009
//*  Copyright  Jan Krause
//*  Email issjagut [ at ] gmail.com
//****************************************************************************/
/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
//@{

#include <stdlib.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <util/delay.h>

#include "config.h"
#include "eemem.h"
#include "enc28j60.h"
#include "ethernet.h"
#include "arp.h"
#include "udp.h"
#include "dhcpc.h"
#include "etherflash.h"

#if DEBUG_AV
#include <avr/pgmspace.h>
#endif

//#define true (1==1)
//#define false (!true)

uint8_t lineBuffer[46];
uint32_t baseAddress;
uint16_t bytesInBootPage;
uint32_t currentAddress;
struct UDP_SOCKET sock;

void initializeHardware (void);


// Debugging
#if DEBUG_AV

void sendchar (unsigned char Zeichen)
{
	while (!(UCSRA & (1<<UDRE)));
	UDR = Zeichen;
}

void puthexbyte(uint8_t bt)
{
	uint8_t btnibble = (bt >> 4) & 0x0f;
	if (btnibble > 9)
		btnibble += 'a'-10;
	else
		btnibble += '0';
	sendchar(btnibble);

	btnibble = bt & 0x0f;
	if (btnibble > 9)
		btnibble += 'a'-10;
	else
		btnibble += '0';
	sendchar(btnibble);
}

void putstring (unsigned char *string)
{
	while (*string)
	{ 
		sendchar (*string);
		string++;
	}
}

void putPGMstring (const char *string)
{
	unsigned char c = 0;
	do
	{
		c = pgm_read_byte(string);
		sendchar (c);
		string++;
	} while (c!='\n');
}
#endif


uint8_t hexToByte(uint8_t *buf, uint16_t idx)
{
  uint8_t val = 0;
  uint8_t i, t;
  for (i=0; i<2; i++)
  {
    t = buf[idx+i];
    if (t > (uint8_t)'9')
    {
      // hex digit a-f
      val = (val<<4) + t - (uint8_t)'A' + 10;
    } else
    {
      // digit 1-9
      val = (val<<4) + t - (uint8_t)'0';
    }
  }    
  return val;
}

// interrupts have to be disabled when calling this function!
void writeFLASHPage(uint32_t currentAddress)
{
	#if DEBUG_AV
		putpgmstring("writeFLASHPage\r\n");
	#else
	    eeprom_busy_wait ();

	    boot_page_erase (currentAddress-2);		// Clear flash page
	    boot_spm_busy_wait ();      			// Wait until the memory is erased.					
	
	    boot_page_write (currentAddress-2);     // Store buffer in flash page.
	    boot_spm_busy_wait();       			// Wait until the memory is written.
	#endif

    bytesInBootPage = 0;
}

// parse a line of the intel hex file. 
// every line starts with a colon, followed by ASCII hex representation of up to 21 data bytes
void processLineBuffer(uint8_t bytes)
{
	// ignore colon byte and decode ASCII hex code to binary
	uint8_t i;
	for (i=0; i<((bytes-3)/2); i++)
	{
		lineBuffer[i] = hexToByte(lineBuffer, (i*2)+1);
	}
			
	uint8_t len;
	
	switch (lineBuffer[3])
	{
		case 0x00: // data record
			len = lineBuffer[0];
			currentAddress = baseAddress + (lineBuffer[1] << 8) + lineBuffer[2];
			// copy data to boot page
			for (i=0; i<len; i+=2)
			{
				#if DEBUG_AV
					putpgmstring("boot_page_fill_safe\r\n");
				#else
				    boot_page_fill_safe (currentAddress, lineBuffer[i+4+0] + (lineBuffer[i+4+1] << 8));
				#endif
				currentAddress += 2;
				bytesInBootPage += 2;
				if (bytesInBootPage == SPM_PAGESIZE)
				{
					// page is full -> write it				
					writeFLASHPage(currentAddress);
				}
			}
			break;
	
		case 0x01: // last record
            // write (incomplete page) if there are bytes in
            if (bytesInBootPage > 0)
            {
                writeFLASHPage(currentAddress);
            }
			break;
			
		case 0x02: // extended segment address record
            // if bytes are in the page buffer, first write the buffer
            if (bytesInBootPage > 0)
            {
                writeFLASHPage(currentAddress);        
            }
			baseAddress = (uint32_t)((lineBuffer[4] << 8) + lineBuffer[5]) << 4;
			break;
			
		case 0x03: // start segment address record
		case 0x05: // start linear address record
			// ignore, we know where to go after the flash
		
			break;
			
		case 0x04: // extended linear address record
			// will never show up on smaller devices (ATmega32, ATmega644) since
			// flash address fits in 16 bits, but in case we have a device with
			// more than 64k flash we need to set the upper adress word here
            // if bytes are in the page buffer, first write the buffer
            if (bytesInBootPage > 0)
            {
                writeFLASHPage(currentAddress);        
            }
			baseAddress = (uint32_t)((lineBuffer[4] << 8) + lineBuffer[5]) << 16;
			break;
	}
	
}

void jumpToApplication(void)
{
    
	// Reenable RWW-section again. We need this if we want to jump back
    // to the application after bootloading.
    boot_rww_enable_safe();

	initializeHardware();

	pApplication();

}


void initializeHardware (void)
{
	// reset hardware register
	// disable TWI
	TWCR &= ~(1<<TWIE);
	// disable INT2
	
	
#ifdef __AVR_ATmega32__
	GICR &= ~(1<<INT2);
#else
	EIMSK = 0;
#endif

	DDRA = 0;
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	PORTA = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;
	// disable SPI
	SPCR &= ~(1<<SPE);
	
}

int main(void)
{
	// disable interrupts
	cli();
	
	initializeHardware();
	
#if DEBUG_AV
	// Debugging über UART (Mega32)
	//DDRD = (1<<PD1);
	//PORTD = 0;
	UCSRB = ( 1 << TXEN );							// UART TX einschalten
	UCSRC |= ( 1 << URSEL )|( 3<<UCSZ0 );	        // Asynchron 8N1
	UBRRH  = 0;                                   	// Highbyte ist 0

#define BAUD 38400L
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)
	UBRRH = UBRR_VAL >> 8;
	UBRRL = UBRR_VAL & 0xFF;
	
	putpgmstring("Start\r\n");
#endif	

	// initialize ENC28J60
	ETH_INIT();
#if DEBUG_AV
	putpgmstring("ETH_INIT\r\n");
#endif
	
	ETH_PACKET_SEND(60,ethernetbuffer);
	ETH_PACKET_SEND(60,ethernetbuffer);
//  enc does not work without a break after init
//	for (uint8_t i=0;i<30;i++)
//		_delay_ms(35);

	// Clear receive buffer of ENC28J60
	while (ETH_PACKET_RECEIVE (MTU_SIZE, ethernetbuffer) != 0 ) {};

	stack_init ();
#if DEBUG_AV
	putpgmstring("stack_init\r\n");
#endif	


	
	
	// init global vars
	baseAddress = 0;
	bytesInBootPage = 0;
	currentAddress = 0;
	
	sock.Bufferfill = 0;
	sock.lineBufferIdx = 0;
	sock.SourcePort = ~TFTP_SERVER_PORT;

	// send RRQ
	uint8_t *udpSendBuffer = ethernetbuffer + (ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN);
	// get Requeststring from EEPROM to save FLASH
	eeprom_read_block ((void*)udpSendBuffer, (const void*)&maTFTPReqStr, TFTPReqStrSize);
	
	UDP_RegisterSocket (sock.SourcePort, (void(*)(unsigned char))tftp_get);
	UDP_SendPacket (TFTPReqStrSize, sock.SourcePort, TFTP_SERVER_PORT, CALC_BROADCAST_ADDR(mlIP, mlNetmask));

	while (1)
	{

		eth_packet_dispatcher();
	
		_delay_ms(2);

	}
	
	return(0);
}


void tftp_get (void)
{
	struct ETH_header * ETH_packet; 		// ETH_struct anlegen
	ETH_packet = (struct ETH_header *) ethernetbuffer;
	struct IP_header * IP_packet;		// IP_struct anlegen
	IP_packet = ( struct IP_header *) &ethernetbuffer[ETH_HDR_LEN];
	struct UDP_header * UDP_packet;		// TCP_struct anlegen
	UDP_packet = ( struct UDP_header *) &ethernetbuffer[ETH_HDR_LEN + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];

	if (sock.Bufferfill == 0)
	{
        arp_entry_add(IP_packet->IP_Srcaddr, ETH_packet->ETH_sourceMac); 
		
		sock.DestinationIP = IP_packet->IP_Srcaddr;

		// Größe der Daten eintragen
		sock.Bufferfill = htons(UDP_packet->UDP_Datalenght) - UDP_HDR_LEN;

		// TFTP: Zielport ändern auf SourcePort des empfangenen Pakets (TID)
		sock.DestinationPort = UDP_packet->UDP_SourcePort;

		// Offset für UDP-Daten im Ethernetfrane berechnen
		sock.DataStartOffset = ETH_HDR_LEN + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 ) + UDP_HDR_LEN;
		
		// check for data packet (00 03)
		if (ethernetbuffer[sock.DataStartOffset+1] == 0x03)
		{	// this is a data packet
			uint16_t rxBufferIdx = sock.DataStartOffset+4;
			// copy current line till newline character or end of rx buf
			while ((rxBufferIdx - sock.DataStartOffset) < sock.Bufferfill)
			{
				// copy next byte from rx buf to line buf
				lineBuffer[sock.lineBufferIdx++] = ethernetbuffer[rxBufferIdx++];
				if (ethernetbuffer[rxBufferIdx-1] == 0x0A)
				{
					// newline
#if DEBUG_AV
	lineBuffer[sock.lineBufferIdx] = 0; // mark end of string
	putstring(lineBuffer);
#endif	
					processLineBuffer(sock.lineBufferIdx);
					sock.lineBufferIdx = 0;
				}
			}
			
			// rx buf processed
			// send ack and wait for next packet
			uint8_t *udpSendBuffer = ethernetbuffer + (ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN);
			udpSendBuffer[0]  = 0x00;
			udpSendBuffer[1]  = 0x04; //TFTP_ACK

			// mark buffer free
			sock.Bufferfill = 0;
			UDP_SendPacket (4, sock.SourcePort, sock.DestinationPort, sock.DestinationIP);
			
			// last packet is shorter than 516 bytes
			if (sock.Bufferfill < 516)
			{
				UDP_UnRegisterSocket(sock.SourcePort);
				// sometimes the hexfile doesn't end with a 0x01 Record (End Of File)
 				// so we have to check if there is unwritten data in the buffer
				if (bytesInBootPage > 0)
				{
					writeFLASHPage(currentAddress);        
				}
#if DEBUG_AV
	putpgmstring("Done\r\n");
#else
				jumpToApplication();
#endif	
			}
		}
		else if (ethernetbuffer[sock.DataStartOffset+1] == 5)
		{
			// error -> reboot to application
			UDP_UnRegisterSocket(sock.SourcePort);
#if DEBUG_AV
	putpgmstring("error\r\n");
#else
			jumpToApplication();
#endif	
		}
	}
}
