call setpath.bat
"nios2-elf-gcc" -S b.cpp -o b.s
compile b.s,inc\irq.asm out.hex b.temp.s dos
pause
