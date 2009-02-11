mkdir fw_bootloader
cd device_001
make clean
make 
cp device_001.hex ../fw_bootloader
cp device_001.eep ../fw_bootloader
cd ..
cd device_002
make clean
make
cp device_002.hex ../fw_bootloader
cp device_002.eep ../fw_bootloader

@pause