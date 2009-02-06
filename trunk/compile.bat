mkdir fw_bootloader
cd device_001
make clean
make 
cp device_001.hex ../fw_bootloader
cp device_001.eep ../fw_bootloader
rem cd ..
rem cd device_002
rem make clean
rem make
rem cp device_002.hex ../fw_bootloader
rem cp device_002.eep ../fw_bootloader

@pause