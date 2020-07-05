call setpath.bat
rem 
"nios2-elf-gcc" -S -Os minesweeper.cpp -o minesweeper.s
compile inc\divsi3.asm,inc\irq.asm,minesweeper.s out.hex minesweeper.temp.s app minesweeper.bin
pause
