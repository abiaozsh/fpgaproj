"gcc\bin\gcc" -S -static-libgcc -msoft-float -masm=intel temp.c -o temp.s
compile temp.s out.hex temp.temp.s
pause
