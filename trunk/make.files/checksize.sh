#!/bin/bash
#----------------------------------------------------------------------------
# Ethernet Bootloader for ATmega section size check script
# written by Andreas Vogt
# v1.0 27.02.09
#
# This script is usually called by make
# It checks the size of the .text section
#

##################################################################
#
# Command line will contain:
# $0	-	The name of the script
# $1	-	The name of the file that contains the actual code sizes, usually size.txt
# $2	-	The maximum size of the .text section, usually 0x3ff
# $3	-	The .text section start address

if [ "$#" -le 1 ]
then	# we have not been called by make, complain
	echo "This script is designed to be invoked by make."
	echo "$MSG_HINT1"
	echo "$MSG_HINT2"
	echo "=> End script <="
	exit 1
fi

curr_size=`gawk '/\.text[[:space:]]+[[:xdigit:]]+/ {print $2}' $1`
let "diff=$2-$curr_size"
if [ "$diff" -le 0 ]
then	# thats bad: .text section is too big, complain and stop build
	echo
	echo
	echo "******* ERROR *******"
	echo "Section .text has a size of $curr_size bytes, but the available space"
	echo "has only $2 bytes."
	echo "Please move more functions to the section .bootloader or increase"
	echo "the space for .text by changing the variable STAT_BLSECSIZE in the"
	echo "makefile. Please note that in this case you have to change the"
	echo "BOOTSZ fuses, too."
	exit 1
fi

# now check size of section .bootloader and calculate start address for
# the second linker run
bl_size=`gawk '/\.bootloader[[:space:]]+[[:xdigit:]]+/ {print $2}' $1`
let "bl_strt_adr=$3-$bl_size"
if [ "$diff" -le 0 ]
then	# thats bad: .text section is too big, complain and stop build
	echo
	echo
	echo "******* ERROR *******"
	echo "Section .bootloader has a size of $bl_size bytes, but the available space"
	echo "has only $3 bytes."
	echo "Please move more functions to the section .text or increase"
	echo "the space for .bootloader by changing the variable STAT_BLSECSIZE in the"
	echo "makefile. Please note that in this case you have to change the"
	echo "BOOTSZ fuses, too."
	exit 1
fi
echo -n "BL_CODE_START  0x" >> "$1"
echo ""$bl_strt_adr" "16" o p" | dc >> "$1"
exit 0

