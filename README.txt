Über die Device_nnn-Unterordner kann man verschiedene Bootloader Versionen kompilieren.
Die Unterordner enthalten die Einstellungen wie IP, MAC und Dateiname für den TFTP Transfer:
-In den Makefiles muss Prozessor und Bootloaderadresse angepasst werden.
-In den Config.h Dateien müssen die Portpins angepasst werden.
-In den eemem.c Dateien müssen die Programmparameter angepasst werden.

Das compile.bat im Hauptordner ruft make in den Geräteordnern auf.

Changelog:

Revision: 30local
Author: jkrause
Date: 23:31:38, Mittwoch, 4. Februar 2009
Message:
ethernetbootloader:
-single run after reboot works fine now.
-problems with call from application (timer1 running, but no packets receiving).


Revision: 29local
Author: jkrause
Date: 00:40:58, Mittwoch, 4. Februar 2009
Message:
ethernetbootloader:
-bugfix: when TFTP packet starts with 0x0D (last packet ends with 0x0A) a '0' was induced into the flash
-bugfix: receive and transmit buffer used same memory. concurrency problems solved.
-still having problems in receivemode: sometimes no processing of arp and udp packets occurs.


Revision: 23local
Author: jkrause
Date: 22:00:50, Donnerstag, 23. Oktober 2008
Message:
added bootloader project



