/*! \file "arp.h" \brief Implementation of the arp protocol */
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

//	#define ARP_ANSWER		0
//	#define NO_ARP_ANSWER	!ARP_ANSWER
	
	struct ARP_TABLE {
		unsigned long IP;
		unsigned char MAC[6];
		uint8_t time;
	};
	
	void arp_reply (void) BOOTLOADER_SECTION;
	void arp_entry_add (unsigned long sourceIP, unsigned char *sourceMac) BOOTLOADER_SECTION;
	unsigned char *arp_entry_search (unsigned long dest_ip) BOOTLOADER_SECTION;



#endif
