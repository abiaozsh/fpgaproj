#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
//#include <windows.h>
#define N 8
#define true 1

//石を置くマスの構造体
typedef struct {
	int x;
	int y;
	int num;
}information;

void DISCcount(char p[][N + 2], int *countX, int *countO); 
void disp(char p[][N + 2]);
int setpos(int y, int x, char c, char p[][N + 2], int on);
int humanset(int *y, int *x, char c, char p[][N + 2]);
int pcset(int *y, int *x, char c, char p[][N + 2]);
void reset(char p[][N + 2]);
void referee(char player, int countX, int countO);

int main(void) {
	char p[N + 2][N + 2];
	int x, y, passcount = 0, countX, countO;
	char c = 'X', pc, player;

	reset(p);
	printf("※X = first.\n");
	printf("(X or O): ");
	scanf("%c", &player);
	if (player == 'X')
		pc = 'O';
	else
		pc = 'X';
	printf("\n");

	while (true) {
		if (c == player) {
			printf("-----------PLAYER-SIDE(%c)-----------\n\n", c);
			disp(p);
			printf("\n");

			if (humanset(&y, &x, player, p) == 1) {		//置く場所があった場合
				passcount = 0;
				c = pc;
			}
			else {		//置く場所がなかった場合
				passcount++;
				printf("Player Pass...\n\n");
				sleep(1);
				c = pc;
				if (passcount == 2)		//2回連続パスのとき
					break;
			}
		}
		else if (c == pc) {
			printf("-------------PC-SIDE(%c)-------------\n\n", c);
			disp(p);
			printf("\n");

			if (pcset(&y, &x, pc, p) == 1) {		//置く場所があった場合
				passcount = 0;
				c = player;
			}
			else {		//置く場所がなかった場合
				passcount++;
				printf("PC Pass...\n\n");
				sleep(1);
				c = player;
				if (passcount == 2)		//2回連続パスのとき
					break;
			}
		}
	}

	DISCcount(p, &countX, &countO);
	referee(player, countX, countO);

	return 0;
}


//盤面の石の数をカウントする関数
void DISCcount(char p[][N + 2], int *countX, int *countO) {
	int X = 0, O = 0;

	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			if (p[i][j] == 'X')
				X++;
			else if (p[i][j] == 'O')
				O++;
		}
	}

	*countX = X;
	*countO = O;
}

//盤面を表示する関数
void disp(char p[][N + 2]) {
	int countX, countO;

	printf("   |");
	for (int i = 0; i < N; i++)
		printf(" %c |", (char)('A' + i));
	for (int i = 1; i <= N; i++) {
		printf("\n");
		for (int j = 0; j <= N; j++)
			printf("---+");
		printf("\n %d |", i);
		for (int j = 1; j <= N; j++)
			printf(" %c |", p[i][j]);
	}
	printf("\n");
	for (int j = 0; j <= N; j++)
		printf("---+");
	printf("\n\n");

	DISCcount(p, &countX, &countO);
	printf("( X:%d  /  O:%d )\n", countX, countO);
}

//裏返せる数をカウントする関数(裏返し処理可能)
int setpos(int y, int x, char c, char p[][N + 2], int on) {
	int vecY;
	int vecX;
	int reverseEach;
	int reverseTotal = 0;
	int i, j, a, b;
	char nc;

	if (c == 'O')
		nc = 'X';
	else if (c == 'X')
		nc = 'O';

	for (vecY = -1; vecY <= 1; vecY++) {
		for (vecX = -1; vecX <= 1; vecX++) {
			if (vecY == 0 && vecX == 0) continue;

			reverseEach = 0;

			for (i = y + vecY, j = x + vecX; p[i][j] != ' '; i += vecY, j += vecX) {
				if (p[i][j] == nc) {
					reverseEach++;
				}
				else {
					if (reverseEach == 0) {
						break;
					}
					else {
						reverseTotal += reverseEach;
						if (on) {
							p[y][x] = c;
							for (a = y + vecY, b = x + vecX; p[a][b] == nc; a += vecY, b += vecX) {
								p[a][b] = c;
							}
						}
						break;
					}
				}
			}
		}
	}
	return reverseTotal;
}

//プレーヤー側の処理を行う関数
int humanset(int *y, int *x, char c, char p[][N + 2]) {
	int flag, count = 0;
	char chax, chay;

	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			if (setpos(i, j, c, p, 0) != 0)
				count++;
		}
	}

	if (count == 0)
		return -1;

	while (true) {
		printf("Please enter the coordinates.\n(ex. A1): ");
		chax = '8';
		chay = 'H';
		while ((chax < 'A') || (chax > 'H')) chax = getchar();
		while ((chay < '1') || (chay > '8')) chay = getchar();
		for (int i = 0; i < N; i++) {
			if (chax == ('A' + i)) *x = i + 1;
			if (chay == ('1' + i)) *y = i + 1;
		}

		flag = setpos(*y, *x, c, p, 1);

		if (flag) {
			printf("\n");
			return 1;
		}

		puts("Can't Reverse!\n");
	}
}

//pc側の処理を行う関数
int pcset(int *y, int *x, char c, char p[][N + 2]) {
	information *info;
	information temp;
	int a = 0;

	info = (information *)malloc(sizeof(information));
	if (info == NULL) {
		puts("Error1");
		return 0;
	}

	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			if (setpos(i, j, c, p, 0) != 0) {
				a++;
				info = (information*)realloc(info, sizeof(information) * a);
				if (info == NULL) {
					puts("Error2");
					return 0;
				}

				info[a - 1].x = j;
				info[a - 1].y = i;
				info[a - 1].num = setpos(i, j, c, p, 0);
			}
		}
	}

	if (a == 0)
		return -1;

	for (int i = 0; i < a - 1; i++) {
		for (int j = 0; j < a - 1 - i; j++) {
			if (info[j].num > info[j + 1].num) {
				temp = info[j];
				info[j] = info[j + 1];
				info[j + 1] = temp;
			}
		}
	}

	*x = info[a - 1].x;
	*y = info[a - 1].y;

	printf("Please enter the coordinates.\n(ex. A1): ");
	sleep(1);
	printf("%c%d\n", 'A' + (char)(*x - 1), *y);
	sleep(1);

	setpos(*y, *x, c, p, 1);
	free(info);
	printf("\n");

	return 1;
}

//盤面を初期化する関数
void reset(char p[][N + 2]) {
	for (int i = 0; i < N + 2; i++) {
		for (int j = 0; j < N + 2; j++) {
			p[i][j] = ' ';
		}
	}
	p[4][4] = 'O';
	p[4][5] = 'X';
	p[5][4] = 'X';
	p[5][5] = 'O';
}

//勝敗を判定する関数
void referee(char player, int countX, int countO) {
	if (player == 'X') {
		if (countX > countO) {
			printf("YOU WIN!!\n");
		}
		else if (countX < countO) {
			printf("YOU LOSE...\n");
		}
		else if (countX == countO) {
			printf("DRAW!\n");
		}
	}
	else if (player == 'O') {
		if (countO > countX) {
			printf("YOU WIN!!\n");
		}
		else if (countO < countX) {
			printf("YOU LOSE...\n");
		}
		else if (countO == countX) {
			printf("DRAW!\n");
		}
	}
}