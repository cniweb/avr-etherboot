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
uint16_t tftpTimeoutCounter;

// local functions
void initializeHardware (void) BOOTLOADER_SECTION;
void sendTFTPrequest(void) BOOTLOADER_SECTION;

void initializeHardware (void)
{
	// reset hardware register
	// disable TWI
	TWCR &= ~(1<<TWIE);

	// disable INT2
#if defined GICR && defined INT2
	GICR &= ~(1<<INT2);
#elif defined EIMSK
	EIMSK = 0;
#endif

#ifdef PORTA
	DDRA = 0;
	PORTA = 0;
#endif
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
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
	
	putpgmstring("\r\n");
	putpgmstring("***********************\r\n");
	putpgmstring("Start\r\n");
#endif	

	// initialize ENC28J60
	ETH_INIT();
#if DEBUG_AV
	putpgmstring("ETH_INIT\r\n");
#endif
	
	ETH_PACKET_SEND(60,ethernetbuffer);
	ETH_PACKET_SEND(60,ethernetbuffer);

	// Clear receive buffer of ENC28J60
	while (ETH_PACKET_RECEIVE (MTU_SIZE, ethernetbuffer) != 0 ) {};

	stack_init ();
#if DEBUG_AV
	putpgmstring("stack_init done\r\n");
#endif	


// at this point we should have a valid IP-Address and be ready
// to execute our Application or to start the boot loader

// do something here to make a decision where to go
	
// for now, we're just jumping to the boot loader
	BootLoaderMain();	// pBootloader();
	
// this call will never return. BootLoader will reset device when done

	return(0);
}

void BootLoaderMain(void)
{
	uint8_t nRetryCounter = 0;

	// init global vars
	baseAddress = 0;
	bytesInBootPage = 0;
	currentAddress = 0;
	tftpTimeoutCounter = 0;
	
	sock.DestinationIP = 0;
	sock.Bufferfill = 0;
	sock.BlockNumber = 0;
	sock.lineBufferIdx = 0;
	sock.SourcePort = ~TFTP_SERVER_PORT;

	// send initial TFTP RRQ
	UDP_RegisterSocket (sock.SourcePort, (void(*)(void))tftp_get);
	sendTFTPrequest();
	
	while (1)
	{

		eth_packet_dispatcher();
	
		_delay_ms(2);
		
		if (tftpTimeoutCounter++ > TFTP_TIMEOUT)
		{
#if DEBUG_AV && DEBUG_TFTP
	putpgmstring("TFTP timeout\r\n");
#endif	
			if ((sock.DestinationIP != 0) && (nRetryCounter++ < 4))
			{	// ok, we had contact to a server, may be it was
				// the first contact to discover the ip.
				// Try again, but not unlimited
				tftpTimeoutCounter = 0;
				sendTFTPrequest();
			}
			else
#if DEBUG_AV
	tftpTimeoutCounter = 0;	// stay in this loop, do nothing
#else
				jumpToApplication();
#endif	
		}
		
	}
	
}

void sendTFTPrequest(void)
{
	
	uint8_t *udpSendBuffer;
	uint8_t reqSize;
	
	udpSendBuffer = ethernetbuffer + (ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN);
	
	#ifdef DHCP_PARSE_TFTP_PARAMS
		uint8_t *file;
		
		//if (dhcp_res.btStat.bStatTFTPfileName)
		//{
			
			file = msTFTPfileName;
			reqSize = 0;
			*udpSendBuffer++ = 0x00; *udpSendBuffer++ = 0x01;
			while (*file)
			{
				*udpSendBuffer++ = *file++;
				reqSize++;
			}
			*udpSendBuffer++ = 0x00;
			*udpSendBuffer++ = 'o';
			*udpSendBuffer++ = 'c';
			*udpSendBuffer++ = 't';
			*udpSendBuffer++ = 'e';
			*udpSendBuffer++ = 't';
			*udpSendBuffer++ = 0x00;
			reqSize += 9;
		//}
		//else
		//{
		//	reqSize = TFTPReqStrSize;
		//	eeprom_read_block ((void*)udpSendBuffer, (const void*)&maTFTPReqStr, TFTPReqStrSize);
		//}
		//if (!dhcp_res.btStat.bStatTFTPserver)
		//	mlTFTPip = CALC_BROADCAST_ADDR(mlIP, mlNetmask);
	#else
		// get Requeststring from EEPROM to save FLASH
		eeprom_read_block ((void*)udpSendBuffer, (const void*)&maTFTPReqStr, TFTPReqStrSize);
		reqSize = TFTPReqStrSize;

		#ifdef FIXED_TFTP_SRV
			eeprom_read_block ((void*)&mlTFTPip, (const void*)&mlTFTPipEEP, 4);
			sock.DestinationIP = mlTFTPip;
		#else
			if (!mlTFTPip)
				mlTFTPip = CALC_BROADCAST_ADDR(mlIP, mlNetmask);
		#endif

	#endif

	UDP_SendPacket (reqSize, sock.SourcePort, TFTP_SERVER_PORT, mlTFTPip);		

	
	//UDP_SendPacket (TFTPReqStrSize, sock.SourcePort, TFTP_SERVER_PORT, IP(192,168,2,24));
#if DEBUG_AV && DEBUG_TFTP
	putpgmstring("TFTP RRQ sent\r\n");
#endif	

}



void tftp_get (void)
{
	struct ETH_header * ETH_packet; 		// ETH_struct anlegen
	ETH_packet = (struct ETH_header *) ethernetbuffer;
	struct IP_header * IP_packet;		// IP_struct anlegen
	IP_packet = ( struct IP_header *) &ethernetbuffer[ETH_HDR_LEN];
	struct UDP_header * UDP_packet;
	UDP_packet = ( struct UDP_header *) &ethernetbuffer[ETH_HDR_LEN + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 )];
	//UDP_packet = ( struct UDP_header *) &ethernetbuffer[UDP_OFFSET];
	struct TFTP_RESPONSE *tftp;
	tftp = (struct TFTP_RESPONSE *)&ethernetbuffer[ETH_HDR_LEN + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 ) + UDP_HDR_LEN];

	uint8_t lastPacket = 0;


#if DEBUG_AV && DEBUG_TFTP
	putpgmstring("tftp_get()\r\n");
#endif	

	// Reset timeout counter
	tftpTimeoutCounter = 0;

#ifndef FIXED_TFTP_SRV
	
	if (sock.DestinationIP == 0)
	{
	#ifdef DHCP_PARSE_TFTP_PARAMS
			sock.DestinationIP = IP_packet->IP_Srcaddr;
	#else
		// ok, this is the first answer from this TFTP-Server
		// lets see, if it wants to deliver the file we requested
		if (tftp->op == TFTP_OP_DATA)
		{	// ok, we want to use this server
			// the problem is, that some TFTP-Server will not
			// understand our ACK packet because we used a
			// broadcast to find them. So we save the IP here,
			// diconnect from the TFTP-Server and after a while
			// connect again with the saved IP.
			sock.DestinationIP = IP_packet->IP_Srcaddr;
			mlTFTPip = IP_packet->IP_Srcaddr;
			
			// don't listen to this port anymore, we got what we want
			UDP_UnRegisterSocket(sock.SourcePort--);
			// register a new port
			UDP_RegisterSocket (sock.SourcePort, (void(*)(void))tftp_get);
		}
		// say bye to any server - wether it delivers data or not
	    arp_entry_add(IP_packet->IP_Srcaddr, ETH_packet->ETH_sourceMac); 
		// send Error
		uint8_t *udpSendBuffer = ethernetbuffer + (ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN);
		// get Errorstring from EEPROM to save FLASH
		eeprom_read_block ((void*)udpSendBuffer, (const void*)&maTFTPErrStr, TFTPErrStrSize);
		UDP_SendPacket (TFTPErrStrSize, sock.SourcePort, htons(UDP_packet->UDP_SourcePort), IP_packet->IP_Srcaddr);

		#if DEBUG_AV && DEBUG_TFTP
			putpgmstring("TFTP ERR sent\r\n");
		#endif	
		
		return;
	#endif
	}

#endif
	
	if (sock.DestinationIP != IP_packet->IP_Srcaddr)
	{	// other TFTP-Server is sending data - ignore it.
#if DEBUG_AV && DEBUG_TFTP
	putpgmstring("Ignoring packet from wrong server\r\n");
#endif	
		return;
	}

	// TFTP: Zielport ändern auf SourcePort des empfangenen Pakets (TID)
	sock.DestinationPort = htons(UDP_packet->UDP_SourcePort);

	if ((sock.BlockNumber + 1) != htons(tftp->blockNumber))
	{
		// this block is not expected
		if (htons(tftp->blockNumber) == (sock.BlockNumber))
		{
			// this block is the last block processed -> send ACK again (UDP is unreliable)
			tftp->op = 0x0400;
			// mark buffer free
			sock.Bufferfill = 0;
#if DEBUG_AV && DEBUG_TFTP
	puthexbyte(sock.BlockNumber>>8);
	puthexbyte(sock.BlockNumber);
	putpgmstring(" send block ACK again\r\n");
#endif		
			UDP_SendPacket (4, sock.SourcePort, sock.DestinationPort, sock.DestinationIP);
		}
#if DEBUG_AV && DEBUG_TFTP
		else
		{
			puthexbyte(tftp->blockNumber);
			puthexbyte(tftp->blockNumber>>8);
			putpgmstring("Ignoring packet with wrong block number\r\n");
		}
#endif	
		return;
	}

	// set current block number
	sock.BlockNumber = htons(tftp->blockNumber);


#if DEBUG_AV && DEBUG_TFTP
	puthexbyte(sock.BlockNumber>>8);
	puthexbyte(sock.BlockNumber);
	putpgmstring(" block processing\r\n");
#endif	

	// Größe der Daten eintragen
	sock.Bufferfill = htons(UDP_packet->UDP_Datalenght) - UDP_HDR_LEN;

	// last packet is shorter than 516 bytes
	if (sock.Bufferfill < 516)
		lastPacket = 1;

	// Offset für UDP-Daten im Ethernetfrane berechnen
	
	sock.DataStartOffset = ETH_HDR_LEN + ((IP_packet->IP_Version_Headerlen & 0x0f) * 4 ) + UDP_HDR_LEN;
	
	// check for data packet (00 03)
	//if (ethernetbuffer[sock.DataStartOffset+1] == 0x03)
	if (tftp->op == TFTP_OP_DATA)
	{	// this is a data packet
		uint16_t rxBufferIdx = sock.DataStartOffset+4;
		//uint16_t rxBufferIdx = 0;
		// copy current line till newline character or end of rx buf
		while ((rxBufferIdx - sock.DataStartOffset) < sock.Bufferfill)
		//while ((rxBufferIdx) < (sock.Bufferfill-4))
		{

			// copy next byte from rx buf to line buf
			//lineBuffer[sock.lineBufferIdx++] = tftp->data[rxBufferIdx++]; 
			lineBuffer[sock.lineBufferIdx++] = ethernetbuffer[rxBufferIdx++];
			if (ethernetbuffer[rxBufferIdx-1] == 0x0A)
			//if (tftp->data[rxBufferIdx-1] == 0x0A)
			{
				// newline
#if DEBUG_AV && #DEBUG_FLASH_PROG
	lineBuffer[sock.lineBufferIdx] = 0; // mark end of string
	putstring(lineBuffer);
#endif	
				processLineBuffer(sock.lineBufferIdx);
				sock.lineBufferIdx = 0;
			}
		}
		
		// rx buf processed
		// send ack and wait for next packet
		//uint8_t *udpSendBuffer = ethernetbuffer + (ETH_HDR_LEN + IP_HDR_LEN + UDP_HDR_LEN);
		//uint8_t *udpSendBuffer = &ethernetbuffer[UDP_DATA_START];
		//udpSendBuffer[0]  = 0x00;
		//udpSendBuffer[1]  = 0x04; //TFTP_ACK
		tftp->op = 0x0400;

		// mark buffer free
		sock.Bufferfill = 0;
#if DEBUG_AV && DEBUG_TFTP
	puthexbyte(tftp->blockNumber);
	puthexbyte(tftp->blockNumber>>8);
	putpgmstring(" block ACK sending\r\n");
#endif			
		UDP_SendPacket (4, sock.SourcePort, sock.DestinationPort, sock.DestinationIP);
		
		if (lastPacket)
		{
			UDP_UnRegisterSocket(sock.SourcePort);
			// sometimes the hexfile doesn't end with a 0x01 Record (End Of File)
				// so we have to check if there is unwritten data in the buffer
			if (bytesInBootPage > 0)
			{
				writeFLASHPage(currentAddress);        
			}
#if DEBUG_AV
	#if DEBUG_TFTP
		putpgmstring("TFTP Done\r\n");
	#endif
#else
			jumpToApplication();
#endif	
		}
	}
	//else if (ethernetbuffer[sock.DataStartOffset+1] == 5)
	else if (tftp->op == TFTP_OP_ERR)
	{
		// error -> reboot to application
#if DEBUG_AV
	#if DEBUG_TFTP
		putpgmstring("TFTP error\r\n");
		puthexbyte(tftp->errCode>>8);
		puthexbyte(tftp->errCode);
		putpgmstring("\r\n");
	#endif
#else
		UDP_UnRegisterSocket(sock.SourcePort);
		jumpToApplication();
#endif	
	}
}


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
		#if DEBUG_FLASH_PROG
		putpgmstring("writeFLASHPage\r\n");
		#endif
	#else
	    eeprom_busy_wait ();

	    boot_page_erase (currentAddress-2);		// Clear flash page
	    boot_spm_busy_wait ();      			// Wait until the memory is erased.					
	
	    boot_page_write (currentAddress-2);     // Store buffer in flash page.
	    boot_spm_busy_wait();       			// Wait until the memory is written.
		boot_rww_enable(); 						// Parts of the bootloader code may
												// be in the RWW section, so make sure
												// we can access it. Thanks to
												// Dirk Armbrust for the hint.
	#endif

    bytesInBootPage = 0;
}

void FillFlashPage(uint32_t currentAddress, uint8_t loByte, uint8_t hiByte)
{	// All SPM instructions must be in the NRWW section
	boot_page_fill_safe(currentAddress, loByte + (hiByte << 8));
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
			currentAddress = baseAddress + (uint32_t)((lineBuffer[1] << 8) + lineBuffer[2]);
			// copy data to boot page
			for (i=0; i<len; i+=2)
			{
				#if DEBUG_AV
					#if DEBUG_FLASH_PROG
					putpgmstring("boot_page_fill_safe\r\n");
					#endif
				#else
				    FillFlashPage (currentAddress, lineBuffer[i+4+0], lineBuffer[i+4+1]);
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

