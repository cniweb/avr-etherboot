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

#include <avr/io.h>
#include <avr/wdt.h>
//#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/boot.h>


#include "config.h"
#include "eemem.h"
#include "ethernet.h"
#include "network.h"
#include "udp.h"
#include "etherflash.h"
#include "enc28j60.h"

#define true (1==1)
#define false (!true)

uint8_t *macUDPBufferTFTP_send;

uint16_t i;

uint8_t lineBuffer[46];
uint32_t currentAddress = 0;
uint16_t bytesInBootPage = 0;

void initializeHardware (void);

inline uint8_t hexToByte(uint8_t *buf, uint16_t idx)
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
void writeFLASHPage(void)
{

    eeprom_busy_wait ();

    boot_page_erase_safe (currentAddress-2);		// Clear flash page
    boot_spm_busy_wait ();      			// Wait until the memory is erased.					
	
    boot_page_write_safe (currentAddress-2);     // Store buffer in flash page.
    boot_spm_busy_wait();       			// Wait until the memory is written.

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
			// copy data to boot page
			for (i=0; i<len; i+=2)
			{
                boot_page_fill_safe (currentAddress, lineBuffer[i+4+0] + (lineBuffer[i+4+1] << 8));
				currentAddress += 2;
				bytesInBootPage += 2;
				if (bytesInBootPage == SPM_PAGESIZE)
				{
					// page is full -> write it				
					writeFLASHPage();
				}
			}
			break;
	
		case 0x01: // last record
            // write (incomplete page) if there are bytes in
            if (bytesInBootPage > 0)
            {
                writeFLASHPage();
            }
			break;
			
		case 0x02: // extended segment address record
            // if bytes are in the page buffer, first write the buffer
            if (bytesInBootPage > 0)
            {
                writeFLASHPage();        
            }
			currentAddress = ((lineBuffer[4] << 8) + lineBuffer[5]) << 4;
			break;
			
		case 0x03: // start segment address record
		
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
	GICR &= ~(1<<INT2);
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
	
	//DDRD |= (1<<PD2);
	//DDRA |= 1+2+4+8+16;

	initializeHardware();
	

	// ENC Initialisieren //
	ETH_INIT();

	// Alle Packet lesen und in leere laufen lassen damit ein definierter zustand herrscht
	while (ETH_PACKET_RECEIVE (MTU_SIZE, ethernetbuffer) != 0 ) {};

	ip_init ();

	for (i=0;i<30;i++)
		_delay_ms(35);

	
	UDP_RegisterSocket (IP(255,255,255,255), 69);

	macUDPBufferTFTP_send = (ethernetbuffer_send + (ETHERNET_HEADER_LENGTH + 20 + UDP_HEADER_LENGTH));
	
	uint8_t lineBufferIdx;
	uint16_t rxBufferIdx;
	uint8_t lastPacket;

	lineBufferIdx = 0;
	lastPacket = false;


	// send RRQ
	// Requeststring liegt im EEPROM, um FLASH zu sparen
	eeprom_read_block ((void*)macUDPBufferTFTP_send, (const void*)&maTFTPReqStr, 12);
	
	sock.Bufferfill = 0;
	UDP_SendPacket (12);

	// prepare send buffer for TFTP ACK packets
	macUDPBufferTFTP_send[0]  = 0x00;
	macUDPBufferTFTP_send[1]  = 0x04; //TFTP_ACK

	while (1)
	{

		ethernet();
		
		if (sock.Bufferfill > 0)
		{
			// check for data packet (00 03)
			if (ethernetbuffer[sock.DataStartOffset+1] == 0x03)
			{ 
												
				// this is a data packet
									
				rxBufferIdx = sock.DataStartOffset+4;
				// copy current line till newline character or end of rx buf
				while (1)
				{
					if ((rxBufferIdx+1) > sock.Bufferfill)
					{
						// rx buf processed
						// send ack and wait for next packet
						macUDPBufferTFTP_send[2]  = ethernetbuffer[sock.DataStartOffset+2];
						macUDPBufferTFTP_send[3]  = ethernetbuffer[sock.DataStartOffset+3];

						// last packet is shorter than 516 bytes
						lastPacket = (sock.Bufferfill < 516);
					
						// mark buffer free
						sock.Bufferfill = 0;
						UDP_SendPacket (4);					
						break;
					}
					else
					{
						// copy next byte from rx buf to line buf
						lineBuffer[lineBufferIdx++] = ethernetbuffer[rxBufferIdx++];
						if (ethernetbuffer[rxBufferIdx-1] == 0x0A)
						{
							// newline
							processLineBuffer(lineBufferIdx);
							lineBufferIdx = 0;
						}
					}
					
				}
				
				if (lastPacket)
				{
					jumpToApplication();
				}
				

			}
			else if (ethernetbuffer[sock.DataStartOffset+1] == 5)
			{
				// error -> reboot to application
				jumpToApplication();
			}
		
		}
		else
		{
			_delay_ms(2);
		}

	}
	
	return(0);

}
