#!/bin/bash
#----------------------------------------------------------------------------
# Ethernet Bootloader for ATmega configuration script
# written by Andreas Vogt
# v1.0 27.02.09
#
# This script is usually called by make
#

# We will export the following variables to makefile.in
# MCU
# F_CPU
# EMB_BOOTLOADER_SECTION_START
# EMB_LINKER_SCRIPT
# EMB_BOOTLOADER_FLAVOR
# EMB_FLASHEND (not needed?)


# Define static values

EMBINIT_FILE="makefile.in"		# make will include this, once we created it
EMBDEFS_FILE="embdefs.txt"		# the compiler will generate this
EMBFOO_FILE="foo.c"				# we create a little file for the compiler to compile

# define options to choose from
STAT_MCU="atmega8 atmega16 atmega32 atmega64 atmega128 atmega2560 atmega2561"

STAT_FREQ="1000000 1843200 2000000 3686400 4000000 7372800 8000000 11059200 \
14745600 16000000 18432000 20000000"

STAT_FLAVOR_FULL="Small Medium Large"
STAT_FLAVOR_PART="Small Medium"

#STAT_FLAVOR_SMALL="1"
#STAT_FLAVOR_MEDIUM="2"
#STAT_FLAVOR_LARGE="3"


# Define Messages
# English
MSG_BEGIN1="****************************************************"
MSG_BEGIN2="*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*"
MSG_BEGIN3="Welcome to the Ethernet Bootloader for ATmega maker!"
MSG_CHDEVICE1="Please select your device."
MSG_CHDEVICE2="Choose from the list below and enter the digit that matches your "
MSG_CHDEVICE3="device and hit return. If your device is not listed, you also may "
MSG_CHDEVICE4="enter a name. However, there is no guarantee that this bootloader "
MSG_CHDEVICE5="will work with your device."
MSG_CHFREQ1="Please select the clock frequency your device runs with."
MSG_CHFREQ2="Choose from the list below and enter the letter that matches your device"
MSG_CHFREQ3="or enter your frequency in Hz if it is not in the list. Then hit return:"
MSG_CHFLAVOR1="Please select the type of bootloader you would like to build."
MSG_CHFLAVOR2="- Small Version, uses 4KB of flash, includes all basic bootlader functions"
MSG_CHFLAVOR3="- Medium Version, uses 6KB of flash, includes all from small version plus DHCP and UDP"
MSG_CHFLAVOR4a="- Large Version, uses 8KB of flash, includes all from medium version plus TCP and Telnet"
MSG_CHFLAVOR4b="(Large Version is not available for your device)"
MSG_HINT1="If you wish to change the configuration of the bootloader"
MSG_HINT2="please type 'make distclean' and then 'make' on the command line."
STAT_ERROR1="ERROR - Please choose one of the given options by entering a number"


function yesorno()
{
	echo "$1"
	read user_reply
	if [[ "$user_reply" == "y" || "$user_reply" == "Y" ]]
	then
		return 0
	fi
	return 1
}

function doerrexit()
{
	echo "=> EXIT <="
	rm -f "$EMBDEFS_FILE"
	rm -f "$EMBINIT_FILE"	# make will stop with an error if this file is missing
	rm -f "$EMBFOO_FILE"
	exit "$1"
}

function sel_device()
{
	echo
	echo "$MSG_CHDEVICE1"
	echo "$MSG_CHDEVICE2"
	echo "$MSG_CHDEVICE3"
	echo "$MSG_CHDEVICE4"
	echo "$MSG_CHDEVICE5"
	echo
	# we need a neat little piece of code for the compiler to get the
	# defined values for the device
	echo "#include <avr/io.h>" > "$EMBFOO_FILE"
	select emb_reply in $STAT_MCU
	do
		echo "=> checking MCU..."
		if [ ${#emb_reply} -eq 0 ]
		then	# a device that is not in the list has been choosen
			emb_device="$REPLY"
		else
			emb_device="$emb_reply"
		fi
		$1 -mmcu=$emb_device -dM -E "$EMBFOO_FILE" > "$EMBDEFS_FILE"
		if [ "$?" -eq 0 ]
		then	# compiler worked without error
			emb_flashend=`gawk '/#define[[:space:]]+FLASHEND[[:space:]]+[[:xdigit:]]+/ {print $3}' $EMBDEFS_FILE`
			if [ ${#emb_flashend} -eq 0 ]
			then	# problem: unknown device?
				echo "ERROR: Sorry, device $emb_device seems to be unknown to the compiler, please choose another one."
			else	# everything seems to be fine
				emb_arch4ld=`gawk '/#define[[:space:]]+__AVR_ARCH__[[:space:]]+[[:digit:]]+/ {print $3}' $EMBDEFS_FILE`
				if [[ "$emb_arch4ld" -eq 4 || "$emb_arch4ld" -eq 5  || "$emb_arch4ld" -eq 51 || "$emb_arch4ld" -eq 6 ]]
				then	# for now, we only support devices of type avr4, avr5 and avr51
					echo "MCU = $emb_device" >> "$EMBINIT_FILE"
					echo "EMB_FLASHEND = $emb_flashend" >> "$EMBINIT_FILE"
					let "emb_blsecstart=$emb_flashend-$3"
					echo -n "EMB_BOOTLOADER_SECTION_START = 0x" >> "$EMBINIT_FILE"
					echo ""$emb_blsecstart" "16" o p" | dc >> "$EMBINIT_FILE"
					printf "EMB_LINKER_SCRIPT = -T %s/eth-avr%s.x\n" "$2" "$emb_arch4ld" >> "$EMBINIT_FILE"
					#todo
					# define XRAMEND 0x45F
					# define RAMEND 0x45F
					# define E2END 0x1FF
					# define LFUSE_DEFAULT (FUSE_SUT0 & FUSE_CKSEL3 & FUSE_CKSEL2 & FUSE_CKSEL1)
					# define HFUSE_DEFAULT (FUSE_SPIEN & FUSE_BOOTSZ1 & FUSE_BOOTSZ0)
					# define FUSEMEM __attribute__((section (".fuse")))
					# define FUSE_BOOTRST (unsigned char)~_BV(0)
					# define FUSE_BOOTSZ0 (unsigned char)~_BV(1)
					# define FUSE_BOOTSZ1 (unsigned char)~_BV(2)
					rm -f "$EMBDEFS_FILE"	# do not need this file any longer
					rm -f "$EMBFOO_FILE"	# do not need this file any longer
					echo "=> MCU = $emb_device OK"
					return $emb_arch4ld;
				else
					echo "ERROR: Sorry, your device is not supported by this version of the "
					echo "bootloader. If you would like to try another device "
					yesorno "type 'y', or 'n' to exit make."
					if [ "$?" -ne 0 ]
					then	# user gives up
						doerrexit 1
					fi
				fi
			fi
		else	# compiler returned an error
			if [ ${#emb_reply} -eq 0 ]
			then	# maybe the device does not exist
				echo "ERROR: Sorry, cannot compile for device $emb_device, please choose another one."
			else	# uh, whats up?
				echo "ERROR: Sorry, cannot compile. Please make sure avr-gcc is properly "
				echo "installed and all environment variables are set. Do not use folder "
				echo "names that contain spaces."
				doerrexit 1
			fi
		fi
	done
}

function sel_freq()
{
	echo
	echo "$MSG_CHFREQ1"
	echo "$MSG_CHFREQ2"
	echo "$MSG_CHFREQ3"
	echo
	select emb_freq in $STAT_FREQ
	do
#		isdigit=`expr match "$REPLY" '\([0-9]*\)'`
#		echo "isdigit => $isdigit"
		if [ ${#emb_freq} -ne 0 ]
		then	# valid number has been choosen
			echo "=> F_CPU = $emb_freq"
			echo "F_CPU = $emb_freq" >> "$EMBINIT_FILE"
			break
		elif [[ "$REPLY" == `expr match "$REPLY" '\([0-9]*\)'` && 10000 -le "$REPLY" ]]
		then	# does it make sense to have a frequency value of less than 10kHz? I quess, no ...
			echo
			echo "Are you sure your device runs with $REPLY Hz ?"
			yesorno "Type 'y' to continue with this value or 'n' to try again."
			if [ "$?" -eq 0 ]
			then	# ok, take this value
				echo "=> F_CPU = $REPLY"
				echo "F_CPU = $REPLY" >> "$EMBINIT_FILE"
				break
			fi
		else
			echo "$STAT_ERROR1"
		fi
	done
}

function sel_flavor()
{
	echo
	echo "$MSG_CHFLAVOR1"
	echo "$MSG_CHFLAVOR2"
	echo "$MSG_CHFLAVOR3"
	if [[ "$1" -eq 4 ]]
	then	# tiny megas to not support large bootloaders
		echo "$MSG_CHFLAVOR4b"
		emb_query_flavor="$STAT_FLAVOR_PART"
	else
		echo "$MSG_CHFLAVOR4a"
		emb_query_flavor="$STAT_FLAVOR_FULL"
	fi
	echo
	select emb_flavor in $emb_query_flavor
	do
		if [ ${#emb_flavor} -ne 0 ]
		then	# valid number has been choosen
			echo "EMB_BOOTLOADER_FLAVOR = $REPLY" >> "$EMBINIT_FILE"
			break
		else
			echo "$STAT_ERROR1"
		fi
	done
}



##################################################################
#
# Command line will contain:
# $0	-	The name of the script
# $1	-	The name of the compiler, usually avr-gcc
# $2	-	Path to the linker script files, usually ../make.files
# $3	-	The size of the .text section, usually 0x3ff

# main()
if [ "$#" -le 1 ]
then	# we have not been called by make, complain
	echo "This script is designed to be invoked by make."
	echo "$MSG_HINT1"
	echo "$MSG_HINT2"
	echo "=> End script <="
	exit 1
fi

echo
echo "$MSG_BEGIN1"
echo "$MSG_BEGIN2"
echo "$MSG_BEGIN3"
echo
#todo read in old definitions and offer as default values
# write header line
echo "#" > "$EMBINIT_FILE"		# !! deletes everything that was in this file
echo "# Please note that this file has been generated by make." >> "$EMBINIT_FILE"
echo "# DO NOT EDIT THIS FILE" >> "$EMBINIT_FILE"
echo "# $MSG_HINT1" >> "$EMBINIT_FILE"
echo "# $MSG_HINT2" >> "$EMBINIT_FILE"
echo "#" >> "$EMBINIT_FILE"
sel_device $1 $2 $3		# returns architecture choosen by user
usr_sel_arch=$?			# remember the architecture
sel_freq
sel_flavor $usr_sel_arch

# todo
# read MAC
# read IP
# read DNS name
# read TFTP server ip
# read application file name

exit 0

