"gcc\bin\gcc" -S -static-libgcc -msoft-float -masm=intel soft-fp\mulsf3.c -o soft-fp\mulsf3.s
rem compile f.s out.hex f.temp.s
pause
