call setpath.bat
"nios2-elf-gcc" -S f.cpp -Os -o f.s
"nios2-elf-gcc" -S inc\vfprintf.c -Os -o vfprintf.s
compile f.s,vfprintf.s out.hex f.temp.s dos BOOT.BIN
pause
