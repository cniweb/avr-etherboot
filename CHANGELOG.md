# Changelog

## Revision: 32

### Author: cniweb

### Date: 22:07, Dienstag, 21. November 2017

### Message:

#### ethernetbootloader:

- bugfix: change currentAddress to uint32_t.


## Revision: 31

### Author: cniweb

### Date: 10:43, Dienstag, 14. November 2017

### Message:

#### ethernetbootloader:

- add travis CI-build.


## Revision: 30local

### Author: jkrause

### Date: 23:31:38, Mittwoch, 4. Februar 2009

### Message:

#### ethernetbootloader:

- single run after reboot works fine now.
- problems with call from application (timer1 running, but no packets receiving).


## Revision: 29local

### Author: jkrause

### Date: 00:40:58, Mittwoch, 4. Februar 2009

### Message:

#### ethernetbootloader:

- bugfix: when TFTP packet starts with 0x0D (last packet ends with 0x0A) a '0' was induced into the flash
- bugfix: receive and transmit buffer used same memory. concurrency problems solved.
- still having problems in receivemode: sometimes no processing of arp and udp packets occurs.


## Revision: 23local

### Author: jkrause

### Date: 22:00:50, Donnerstag, 23. Oktober 2008

### Message:

- added bootloader project
