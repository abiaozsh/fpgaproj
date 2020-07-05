call setpath.bat
"nios2-elf-gcc" -S -Os a.cpp -o a.s
compile a.s,inc\irq.asm out.hex a.temp.s dos a.bin
pause
