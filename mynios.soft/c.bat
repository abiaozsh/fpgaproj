rem "nios2-elf-gcc" -E inc/soft-fp/muldf3.c -o muldf3.temp.c
call setpath.bat
rem -mhw-div 
"nios2-elf-gcc" -Os -S inc/divsi3.c -o divsi3.s
"nios2-elf-gcc" -Os -S inc/soft-fp/float.c -o float.s
"nios2-elf-gcc" -Os -S c.cpp -o c.s
compile float.s,divsi3.s,c.s,inc\irq.asm out.hex c.temp.s dos
pause
