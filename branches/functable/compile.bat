mkdir fw_bootloader
rem cd device_001
rem make clean
rem make 
rem cp device_001.hex ../fw_bootloader
rem cp device_001.eep ../fw_bootloader
rem cd ..
cd device_002
make clean
make
cp device_002.hex ../fw_bootloader
cp device_002.eep ../fw_bootloader
rem cd ..
rem cd device_001_Terminal_Deele
rem make clean
rem make
rem cp device_001_Terminal_Deele.hex ../fw_bootloader
rem cp device_001_Terminal_Deele.eep ../fw_bootloader

@pause