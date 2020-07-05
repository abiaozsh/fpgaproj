call setpath.bat
"nios2-elf-gcc" -Os -mhw-div -S z.cpp -o z.s
compile z.s out.hex z.temp.s dos z.bin
pause
