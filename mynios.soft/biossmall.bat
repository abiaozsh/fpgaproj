call setpath.bat
rem "nios2-elf-gcc" -S bios.cpp -Os -o bios.s
compile biossmall.asm out.hex bios.temp.s bios
pause
