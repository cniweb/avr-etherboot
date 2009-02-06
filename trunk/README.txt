Über die Device_nnn-Unterordner kann man verschiedene Bootloader Versionen kompilieren.
Die Unterordner enthalten die Einstellungen wie IP, MAC und Dateiname für den TFTP Transfer:
-In den Makefiles muss Prozessor und Bootloaderadresse angepasst werden.
-In den Config.h Dateien müssen die Portpins angepasst werden.
-In den eemem.c Dateien müssen die Programmparameter angepasst werden.

Das compile.bat im Hauptordner ruft make in den Geräteordnern auf.
