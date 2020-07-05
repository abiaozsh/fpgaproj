call setpath.bat
"nios2-elf-gcc" -S -Os e.cpp -o e.s
compile inc/irq.asm,e.s out.hex e.temp.s dos e.bin
pause
