@echo off
if exist "C:\Program Files (x86)\Arduino" set arduinopath=C:\Program Files (x86)\Arduino
if exist "C:\Program Files\Arduino" set arduinopath=C:\Program Files\Arduino
set comPort=COM6
set filename=blink
rem set comptype=gcc
rem set extptype=c
set comptype=g++
set extptype=cpp

"%arduinopath%\hardware\tools\avr\bin\avr-%comptype%" -Os -fno-inline -mmcu=atmega8 -save-temps -fverbose-asm -I"%arduinopath%\hardware\arduino\cores\arduino" %filename%.%extptype%
"%arduinopath%\hardware\tools\avr\bin\avr-%comptype%" -Os -fno-inline -mmcu=atmega8                           -I"%arduinopath%\hardware\arduino\cores\arduino" %filename%.%extptype% -o %filename%.o
"%arduinopath%\hardware\tools\avr\bin\avr-objcopy" -O ihex %filename%.o %filename%.hex
rem use examples -> ArduinoISP
    "%arduinopath%\hardware\tools\avr\bin\avrdude" -C"%arduinopath%\hardware\tools\avr\etc\avrdude.conf" -patmega8  -cstk500v1 -P\\.\%comPort% -b19200    -Uflash:w:%filename%.hex:i 
rem "%arduinopath%\hardware\tools\avr\bin\avrdude" -C"%arduinopath%\hardware\tools\avr\etc\avrdude.conf" -pattiny24 -cstk500v1 -P\\.\%comPort% -b19200    -Uflash:w:%filename%.hex:i 
rem "UploaderMega8.exe" %comPort% %filename%.hex true 9600
"%arduinopath%\hardware\tools\avr\bin\avr-size" %filename%.hex
pause
del %filename%.hex
del %filename%.ii
del %filename%.o
del %filename%.s
del a.out
pause
