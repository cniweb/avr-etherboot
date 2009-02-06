#include "config.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/boot.h>
#include <avr/interrupt.h>

#include "eemem.h"
#include "ethernet.h"
#include "network.h"
#include "udp.h"
#include "timer1.h"
#include "etherflash.h"
#include "enc28j60.h"

#define true (1==1)
#define false (!true)

//uint8_t *macUDPBufferTFTP;
uint8_t *macUDPBufferTFTP_send;

uint16_t i;

uint8_t lineBuffer[46];
uint32_t currentAddress = 0;
uint16_t bytesInBootPage = 0;

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

void writeFLASHPage(void)
{
    uint8_t sreg;
    // Disable interrupts.
    sreg = SREG;
    cli();

    eeprom_busy_wait ();

    boot_page_erase (currentAddress-2);		// Clear flash page
    boot_spm_busy_wait ();      // Wait until the memory is erased.					

    boot_page_write (currentAddress-2);     // Store buffer in flash page.
    boot_spm_busy_wait();       // Wait until the memory is written.

    // Reenable RWW-section again. We need this if we want to jump back
    // to the application after bootloading.
    boot_rww_enable();

    // Re-enable interrupts (if they were ever enabled).
    SREG = sreg;
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
                boot_page_fill (currentAddress, lineBuffer[i+4+0] + (lineBuffer[i+4+1] << 8));
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

	cli();
	uint8_t temp;
	
	#ifdef __AVR_ATmega2561__ 

		// Get MCUCR
		temp = MCUCR;
		// Enable change of Interrupt Vectors
		MCUCR = (temp | (1<<IVCE));
		// Move interrupts to App Flash section 
		MCUCR = (temp & ~(1<<IVSEL));

	#else
	
		#ifdef __AVR_ATmega644__

			// Get MCUCR
			temp = MCUCR;
			// Enable change of Interrupt Vectors
			MCUCR = (temp | (1<<IVCE));
			// Move interrupts to App Flash section 
			MCUCR = (temp & ~(1<<IVSEL));
		
		#else
	
			// Use GICR on Mega32
			temp = GICR;
			// Enable change of Interrupt Vectors
			GICR = temp|(1<<IVCE);
			// Move interrupts to App Flash section 
			GICR = (temp & ~(1<<IVSEL));

		#endif
		
	#endif

	pApplication();

}

int main(void)
{

	// Move interrupts to bootsection
	volatile uint8_t temp;
	
	#ifdef __AVR_ATmega2561__
	
		// Get MCUCR
		temp = MCUCR;
		// Enable change of Interrupt Vectors
		MCUCR = temp|(1<<IVCE);
		// Move interrupts to Boot Flash section 
		MCUCR = temp|(1<<IVSEL);
		
	#else
		#ifdef __AVR_ATmega644__

			// Get MCUCR
			temp = MCUCR;
			// Enable change of Interrupt Vectors
			MCUCR = temp|(1<<IVCE);
			// Move interrupts to Boot Flash section 
			MCUCR = temp|(1<<IVSEL);

		#else
			// Use GICR on Mega32
			temp = GICR;
			// Enable change of Interrupt Vectors
			GICR = temp|(1<<IVCE);
			// Move interrupts to Boot Flash section 
			GICR = temp|(1<<IVSEL);
		#endif
					
	#endif
	
	//DDRD |= (1<<PD2);
	//DDRA |= 1+2+4+8+16;
	
	timer1_init ();

	sei();

	// Mac-Addresse aus EEPROM lesen
	eeprom_read_block (&maMac, (const void*)&maMacEEP, 6);
	// ENC Initialisieren //
	enc28j60Init();

	// Alle Packet lesen und in leere laufen lassen damit ein definierter zustand herrscht
	while ( getEthernetframe( MAX_FRAMELEN, ethernetbuffer) != 0 ) { };

	// gibt Ethernet frei
	TIMER1_msec_CallbackFunc = ethernet;

	ip_init ();

	for (i=0;i<60;i++)
		_delay_ms(35);

	// LM298 abschalten
	//DDRB |= (1<<PB5) | (1<<PB6);
	//PORTB &= ~((1<<PB5) | (1<<PB6));

	
	UDP_RegisterSocket (IP(255,255,255,255), 69);

	//macUDPBufferTFTP = (ethernetbuffer + (ETHERNET_HEADER_LENGTH + 20 + UDP_HEADER_LENGHT));
	macUDPBufferTFTP_send = (ethernetbuffer_send + (ETHERNET_HEADER_LENGTH + 20 + UDP_HEADER_LENGHT));
	
	uint8_t lineBufferIdx;
	uint16_t rxBufferIdx;
	uint8_t lastPacket;
	//uint16_t liTimeOut;

	lineBufferIdx = 0;
	lastPacket = FALSE;
	//liTimeOut = 0;


	// send RRQ
	
	// Requeststring liegt im EEPROM, um FLASH zu sparen
	eeprom_read_block ((void*)macUDPBufferTFTP_send, (const void*)&maTFTPReqStr, 12);
	
	sock.Bufferfill = 0;
	UDP_SendPacket (12);

	macUDPBufferTFTP_send[0]  = 0x00;
	macUDPBufferTFTP_send[1]  = 0x04; //TFTP_ACK

	while (1)
	{

		if (sock.Bufferfill > 0)
		{
			//liTimeOut = 0;
			
			// check for data packet (00 03)
			if (UDPRxBuffer[1] == 0x03)
			{ 
												
				// this is a data packet
									
				rxBufferIdx = 4;
				// copy current line till newline character or end of rx buf
				while (1)
				{
					if ((rxBufferIdx+1) > sock.Bufferfill)
					{
						// rx buf processed
						// send ack and wait for next packet
						macUDPBufferTFTP_send[2]  = UDPRxBuffer[2];
						macUDPBufferTFTP_send[3]  = UDPRxBuffer[3];

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
						lineBuffer[lineBufferIdx++] = UDPRxBuffer[rxBufferIdx++];
						if (UDPRxBuffer[rxBufferIdx-1] == 0x0A)
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
			else if (UDPRxBuffer[1] == 5)
			{
				// error -> reboot to application
				jumpToApplication();
			}
		
		}
		else
		{
			// no data
			/*
			_delay_ms(1);
			if (++liTimeOut > 4000)
			{
				// Timeout -> reboot to application
				jumpToApplication();
			}
			*/
		}

	}
	
	return(0);

}

/*
EMPTY_INTERRUPT(INT0_vect); //);
EMPTY_INTERRUPT(INT1_vect); //			_vect); //OR(2)
EMPTY_INTERRUPT(INT2_vect); //			_vect); //OR(3)
EMPTY_INTERRUPT(INT3_vect); //			_vect); //OR(4)
EMPTY_INTERRUPT(INT4_vect); //			_vect); //OR(5)
EMPTY_INTERRUPT(INT5_vect); //			_vect); //OR(6)
EMPTY_INTERRUPT(INT6_vect); //			_vect); //OR(7)
EMPTY_INTERRUPT(INT7_vect); //			_vect); //OR(8)

EMPTY_INTERRUPT(PCINT0_vect); //			_vect); //OR(9)
EMPTY_INTERRUPT(PCINT1_vect); //			_vect); //OR(10)
EMPTY_INTERRUPT(_VECTOR(11));
EMPTY_INTERRUPT(WDT_vect); //			_vect); //OR(12)
EMPTY_INTERRUPT(TIMER2_COMPA_vect); //		_vect); //OR(13)
EMPTY_INTERRUPT(TIMER2_COMPB_vect); //		_vect); //OR(14)
EMPTY_INTERRUPT(TIMER2_OVF_vect); //			_vect); //OR(15)
EMPTY_INTERRUPT(TIMER1_CAPT_vect); //		_vect); //OR(16)
EMPTY_INTERRUPT(TIMER1_COMPA_vect); //		_vect); //OR(17)
EMPTY_INTERRUPT(TIMER1_COMPB_vect); //		_vect); //OR(18)
EMPTY_INTERRUPT(TIMER1_COMPC_vect); //		_vect); //OR(19)
//EMPTY_INTERRUPT(TIMER1_OVF_vect); //			_vect); //OR(20)
EMPTY_INTERRUPT(TIMER0_COMPA_vect); //		_vect); //OR(21)
EMPTY_INTERRUPT(TIMER0_COMPB_vect); //		_vect); //OR(22)
EMPTY_INTERRUPT(TIMER0_OVF_vect); //			_vect); //OR(23)
EMPTY_INTERRUPT(SPI_STC_vect); //			_vect); //OR(24)
EMPTY_INTERRUPT(USART0_RX_vect); //			_vect); //OR(25)
EMPTY_INTERRUPT(USART0_UDRE_vect); //		_vect); //OR(26)
EMPTY_INTERRUPT(USART0_TX_vect); //			_vect); //OR(27)
EMPTY_INTERRUPT(ANALOG_COMP_vect); //		_vect); //OR(28)
EMPTY_INTERRUPT(ADC_vect); //			_vect); //OR(29)
EMPTY_INTERRUPT(EE_READY_vect); //			_vect); //OR(30)
EMPTY_INTERRUPT(TIMER3_CAPT_vect); //		_vect); //OR(31)
EMPTY_INTERRUPT(TIMER3_COMPA_vect); //		_vect); //OR(32)
EMPTY_INTERRUPT(TIMER3_COMPB_vect); //		_vect); //OR(33)
EMPTY_INTERRUPT(TIMER3_COMPC_vect); //		_vect); //OR(34)
EMPTY_INTERRUPT(TIMER3_OVF_vect); //			_vect); //OR(35)
EMPTY_INTERRUPT(USART1_RX_vect); //			_vect); //OR(36)
EMPTY_INTERRUPT(USART1_UDRE_vect); //		_vect); //OR(37)
EMPTY_INTERRUPT(USART1_TX_vect); //			_vect); //OR(38)
EMPTY_INTERRUPT(TWI_vect); //			_vect); //OR(39)
EMPTY_INTERRUPT(SPM_READY_vect); //			_vect); //OR(40)
EMPTY_INTERRUPT(TIMER4_COMPA_vect); //		_vect); //OR(42)
EMPTY_INTERRUPT(TIMER4_COMPB_vect); //		_vect); //OR(43)
EMPTY_INTERRUPT(TIMER4_COMPC_vect); //		_vect); //OR(44)
EMPTY_INTERRUPT(TIMER4_OVF_vect); //			_vect); //OR(45)
EMPTY_INTERRUPT(_VECTOR(41));
EMPTY_INTERRUPT(TIMER5_COMPA_vect); //		_vect); //OR(47)
EMPTY_INTERRUPT(TIMER5_COMPB_vect); //		_vect); //OR(48)
EMPTY_INTERRUPT(TIMER5_COMPC_vect); //		_vect); //OR(49)
EMPTY_INTERRUPT(TIMER5_OVF_vect); //			_vect); //OR(50)
EMPTY_INTERRUPT(_VECTOR(46));
*/