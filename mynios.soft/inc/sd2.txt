https://wenku.baidu.com/view/ce5042052af90242a895e582.html
https://www.nxp.com/docs/en/application-note/AN3049.pdf

You shouldn't use CMD1 to initialize SDHC cards; as said in the SD Card specification:

In any of the cases CMD1 is not recommended because it may be difficult for the host to distinguish between MultiMediaCard and SD Memory Card.

Some controllers (newer and higher capacity cards mostly) will simply stay in IDLE if you issue CMD1 to them. You should first issue CMD8 0x1AA after the reset (CMD0), and then use CMD55 + ACMD41.

tl;dr to initialize the card you should:

CMD0 arg: 0x0, CRC: 0x95 (response: 0x01)

CMD8 arg: 0x000001AA, CRC: 0x87 (response: 0x01)

CMD55 arg: 0x0, CRC: any (CMD55 being the prefix to every ACMD)

ACMD41 , arg: 0x40000000, CRC: any

if response: 0x0, you're OK; if it's 0x1, goto 3.

Note that most cards require steps 3/4 to be repeated, usually once, i.e. the actual sequence is CMD0/CMD8/CMD55/ACMD41/CMD55/ACMD41 - to be sure, try it n times (select n within your reason), and assume fail if it doesn't.

Also, note that you should assert S?S? (aka CS) low at least before and after CMD0 - and, in reality, probably always when sending any command to the SD card.