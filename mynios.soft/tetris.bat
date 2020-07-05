call setpath.bat
rem 
"nios2-elf-gcc" -S -Os tetris.cpp -o tetris.s
compile inc\irq.asm,tetris.s out.hex tetris.temp.s app tetris.bin
pause
