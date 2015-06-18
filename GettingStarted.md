# Introduction #

This document describes how to get started using the etherboot bootloader for avr devices.

# Prerequisites #

You need the latest Version of WinAVR to compile the code (At the moment this is 20090313 and can be downloaded [here](http://sourceforge.net/project/showfiles.php?group_id=68108)).

You will need a TFTP server for supplying the application firmware file to the bootloader.

Optionally you need a DHCP Server for supplying an IP address for the device and the IP address of the TFTP server and the filename of the file to be loaded and programmed.

# Configuring the bootloader #

You can select from two different version of the bootloader namely "small" and "medium".
The small version uses fixed ip address for itself and for the TFTP server to contact. The medium version contains a DHCP client implementation which requests the configuration data from a DHCP server.

The selection of the version to build is done by the makefile (see section compiling the code).
To configure MAC address and IP address, TFTP IP address and filename, edit the files eemem.c and config.h in the folder device\_001.

# Compiling the code #

Change to the directory device\_001 and call make.
The first time make ist called, a shellscript is started, which asks you some questions about the version to build and some parameters concerning the mcu type and oscillator frequency. The values are stored in the file makefile.in. You can delete the file, to get asked again when calling make.

Afterwards the sources are compiled to the output files device\_001.hex (FLASH image) and device\_001.eep (EEPROM image).

# Programming the device #

You have to programm both the FLASH (hex-file) and EEPROM (eep-file).
You have to programm the following fuses:
  * BOOTSZ0 = 1 (not programmed), BOOTSZ1 = 1 (not programmed) to select bootsize 512 words.
  * BOOTRST = 0 (programmed) to jump to the bootloadercode after reset and power on.