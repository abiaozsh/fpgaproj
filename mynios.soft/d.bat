call setpath.bat
"nios2-elf-gcc" -S d.cpp -o d.s
compile d.s out.hex d.temp.s dos
pause
