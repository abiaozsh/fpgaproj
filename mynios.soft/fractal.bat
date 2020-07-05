rem "nios2-elf-gcc" -E inc/soft-fp/muldf3.c -o muldf3.temp.c
call setpath.bat
rem -mhw-div 
"nios2-elf-gcc" -Os -S inc/divsi3.c -o divsi3.s
"nios2-elf-gcc" -Os -S inc/soft-fp/float.c -o float.s
"nios2-elf-gcc" -Os -S fractal.cpp -o fractal.s
compile float.s,divsi3.s,fractal.s,inc\irq.asm out.hex fractal.temp.s dos
pause
