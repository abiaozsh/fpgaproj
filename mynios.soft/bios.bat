call setpath.bat
"nios2-elf-gcc" -S bios.cpp -Os -o bios.s
compile bios.s,inc\irq.asm out.hex bios.temp.s bios
pause
