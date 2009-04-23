/*! \file "checksum.c" \brief Berechnet die Checksumme eine Blockes */
//***************************************************************************
//*            checksum.c
//*
//*  Sun Feb  8 2009
//*  Copyright  Andreas Vogt
//*  Email
//****************************************************************************/
///	\ingroup math
///	\defgroup CHECKSUM 16-bit Checksummenfuntion (checksum-c)
///	\code #include "checksum.h" \endcode
///	\par Uebersicht
///		Berechnung der 16.Bit Checksumme eines Speicherbereiches im RAM.
/// Benotig z.B. fuer die Checksumme von Datenpacketen wie in TCP/IP.
/// Das Ergebnis kann direkt übernommen werden, ein htons() ist nicht
/// mehr nötig.
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

#include <stdint.h>
#include <avr/boot.h>
#include "config.h"
#include "checksum.h"
#include "ethernet.h"

uint16_t Checksum_16 (unsigned char * pointer, uint16_t headerlenght, unsigned long startval)
{
	struct TPWORDACC
	{
		uint16_t DataL;
		uint16_t DataH;
	};
	union
	{
		struct TPWORDACC nWordAcc;
		unsigned long lLongAcc;
	} checksum;
	checksum.lLongAcc = htons(startval);
	uint16_t *pP16 = (uint16_t *)pointer;
	
	//Jetzt werden alle Packete in einer While Schleife addiert
	while( headerlenght > 0)
	{
		if( headerlenght == 1)
		{	// headerlenght ist ungerade, füge eine null hinzu (DataH einfach weglassen)
			checksum.lLongAcc += (*pP16 & 0x00ff);
			break;
		}
		else
		{
			checksum.lLongAcc += *pP16++;
			headerlenght -=2 ;
		}
	}

	//Komplementbildung (addiert Long INT_H Word mit Long INT L Word)
	while (checksum.nWordAcc.DataH)
		checksum.lLongAcc = (uint32_t)checksum.nWordAcc.DataL + (uint32_t)checksum.nWordAcc.DataH;
	return ~(checksum.nWordAcc.DataL);
}

//@}
