/*! \file "arp.h" \brief Die Arp-Funktionlitaet */
//***************************************************************************
//*            arp.h
//*
//*  Mon Aug 28 22:31:14 2006
//*  Copyright  2006  sharandac
//*  Email sharandac(at)snafu.de
//****************************************************************************/
///	\ingroup network
///	\defgroup ARP ARP-Funktionen (arp.h)
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
#ifndef __ARP_H__

	#define __ARP_H__

	extern void arp( unsigned int packet_lenght, unsigned char *buffer);
	unsigned int GetIP2MAC( unsigned long IP, unsigned char * MACbuffer );

	#define MAX_ARPTABLE_ENTRYS 1
	
	#define ARP_ANSWER		0
	#define NO_ARP_ANSWER	!ARP_ANSWER
	
	struct ARP_TABLE {
		unsigned long IP;
		unsigned char MAC[6];
	};

	#define ARP_HEADER_LENGHT 28
	
	struct ARP_header {
		unsigned int HWtype;				// 2 Byte
		unsigned int Protocoltype;			// 2 Byte
		unsigned char HWsize;				// 1 Byte
		unsigned char Protocolsize;			// 1 Byte
		unsigned int ARP_Opcode;			// 2 Byte
		unsigned char ARP_sourceMac[6];		// 6 Byte
		unsigned long ARP_sourceIP;			// 4 Byte
		unsigned char ARP_destMac[6];		// 6 Byte
		unsigned long ARP_destIP;			// 4 Byte = 28
	};

#endif
