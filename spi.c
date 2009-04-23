/*!\file spi.c \brief Implementation of SPI Communication */

///	\ingroup hardware
///	\defgroup SPI SPI-Interface (spi.c)
///	\code #include "spi.h" \endcode
///	\par Uebersicht
///		Die SPI-Schnittstelle fuer den AVR-Controller
//****************************************************************************/
//@{
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

#include <avr/io.h>
#include "config.h"
#include "spi.h"
 
/* -----------------------------------------------------------------------------------------------------------*/
/*! Die Init fuer dir SPI-Schnittstelle. Es können verschiedene Geschwindigkeiten eingestellt werden.
 * \param 	Option		Hier kann die Geschwindigkeit der SPI eingestellt werden.
 */
/* -----------------------------------------------------------------------------------------------------------*/
void SPI_init (void)
{

	// MOSI, SCK, SS als Output

	// Enable Pullups
	//SPI_DDR &= ~((1<<MOSI) | (1<<SCK) | (0<<SS));
	//SPI_PORT |= (1<<MOSI) | (1<<SCK) | (0<<SS);
	
	SPI_DDR = 0;
	SPI_PORT = 0xff;
	
	// set mosi, sck as output
	SPI_DDR |= (1<<MOSI) | (1<<SCK); 

	// SCK auf Hi setzen
	// SPI_PORT |= 1<<SCK;

	// Master mode
	SPCR = (1<<MSTR) | (1<<SPE);
	
	// SPI_HALF_SPEED
	SPSR &= ~(1<<SPI2X);
	//SPSR |= 0<<SPI2X;

 	/* SPI_FULL_SPEED
	SPSR |= 1<<SPI2X;
    */
}

/* -----------------------------------------------------------------------------------------------------------*/
/*! Schreibt einen Wert auf den SPI-Bus. Gleichzeitig wird ein Wert von diesem im Takt eingelesen.
 * \warning	Auf den SPI-Bus sollte vorher per Chip-select ein Baustein ausgewaehlt werden. Dies geschied nicht in der SPI-Routine sonden
 * muss von der Aufrufenden Funktion gemacht werden.
 * \param 	Data	Der Wert der uebertragen werden soll.
 * \retval  Data	Der wert der gleichzeit empfangen wurde.
 */
/* -----------------------------------------------------------------------------------------------------------*/
unsigned char SPI_ReadWrite (unsigned char Data)
{
	// daten senden
	SPDR = Data;
	// auf fertig warten

	while (!(SPSR & (1<<SPIF)));

	// empfangende daten einlesen
	Data = SPDR;
	// daten zurueckgeben
			
	return (Data);
}

//@}
