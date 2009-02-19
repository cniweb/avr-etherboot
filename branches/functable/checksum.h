/*! \file checksum.h \brief Berechnet die Checksumme eine Blockes */
/***************************************************************************
 *            checksum.h
 *
*  Sun Feb  8 2009
*  Copyright  Andreas Vogt
 *  Email
 ****************************************************************************/
///	\ingroup math
//@{
#ifndef __CHECKSUM_H__
#define __CHECKSUM_H__
	
	uint16_t Checksum_16 (unsigned char * pointer, uint16_t headerlenght, unsigned long startval) BOOTLOADER_SECTION;

#endif
//@}
