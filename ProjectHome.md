# AVR Etherboot #

An ethernet bootloader for the Atmel AVR microcontroller family.

## Introduction ##

This project implements a bootloader for AVR ATMega 8bit microcontrollers, that loads the firmware via ethernet and uses the TFTP protocol.
The goal is to reach a bootloader size of 2k words maximum to fit into the smaller members of the AVR family.

### Supported devices ###
  * ATMega32
  * ATMega644
  * ATMega64
  * ATMega128
  * ATMega256

Every AVR with at least 512k WORD bootloader flashsize should work.

### Supported ethernet hardware ###
Currently the only supported ethernet hardware is the Microchip ENC28J60 ethernet controller. It communicates via the SPI bus with the ATmega and includes ethernet PHY/MAC in a single chip.


## Functional description ##
We are developing two different versions of the bootloader:
  * a small version which uses fixed addresses for itself and the TFTP server
  * a medium version which implements a dhcp client for getting IP, TFTP-IP and filename of the firmware.
  * (planned) a large version with enhanced features.

When the bootloader starts executing, the following actions take place:
  * Load the parameters from EEPROM
  * Initialize hardware peripherals
  * (optional) Send DHCP request and process reply
  * Send a TFTP Read Request to local IP broadcast address (or to address given by DHCP server)
  * Process TFTP datapackets
    * Receive a datapacket
    * Decode Intel Hex file format
    * Program flash memory pages
  * Jump to application (Address 0x0000)

See [this page](GettingStarted.md) for a short introduction how to get started with the bootloader.