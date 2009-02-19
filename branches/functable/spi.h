/*!\file spi.h \brief Implementation of SPI Communication */
///	\ingroup hardware
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
 
#ifndef _SPI_H
	#define _SPI_H
	
	#include <avr/io.h>

	void SPI_init (void) BOOTLOADER_SECTION;
	unsigned char SPI_ReadWrite (unsigned char Data) BOOTLOADER_SECTION;

	#define SPI_NOT_INIT	0x00
	#define SPI_HALF_SPEED	0x01
	#define SPI_FULL_SPEED	0x02
	
#endif /* _SPI_H */
//@}
