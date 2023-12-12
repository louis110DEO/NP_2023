// 1223.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include"TCPIP_Sync.h"

#include<windows.h>//必須先加入此標頭檔
#include <conio.h>
#define W 52
#define H 32
int T[W][H];//紀錄棋盤狀態，0x00:未下棋，'o':己方棋子，'x':對方旗子
void gotoxy(int xpos, int ypos)
{
	COORD scrn;
	HANDLE hOuput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = xpos; scrn.Y = ypos;
	SetConsoleCursorPosition(hOuput, scrn);
}

void check()//檢查五連線
{
	int x, y, z, n, m, p, ax, ay; 
	int T1[2] = { '0','X' };
	int V[4][2] = { {1,0},{0,1},{1,1},{-1,1} };

	for (z = 0;z < 2;z++)
		for (y = 0; y < H; y += 2)
			for (x = 0; x < W; x += 2)
				for (m = 0; m < 4; m++)
				{
					for (n = 0; n < 10; n += 2)
					{
						ax = x + V[m][0] * n;
						ay = y + V[m][1] * n;
						if ((ax < 0) || (ax >= W) || (ay < 0) || (ay >= H) || (T[ax][ay] != T1[z]))
							break;
					}
					if (n >= 10)
					{
						gotoxy(0, H); 
						printf("%c:五子連線", T1[z]);
					}
				}
			//活三
			//絕四
			//預測雙活三點
}

SOCKET SSock, CSock, Sock;
int F = 0;
void SFun(PVOID p)// Server Mode:處理遠端連入
{
	sockaddr_in Addr;
	int  x, y, i, Len = sizeof(sockaddr);
	Sock = accept(SSock, (sockaddr*)&Addr, &Len); F = 2;// 接受連線
	gotoxy(0, H); printf("遠端(%s)連入", inet_ntoa(Addr.sin_addr));
	// 接受連線
	char s1[2000];
	while (1)
	{
		i = recv(Sock, s1, sizeof(s1) - 1, 0); // 讀取資料
		if (i > 0)
		{
			s1[i] = 0;
			// gotoxy(0, H);
			i = sscanf_s(s1, "(%d-%d)", &x, &y);// 解析下棋位置
			if (i == 2)
			{
				gotoxy(x, y);
				printf("X");
				T[x][y] = 'X';
				check();// 對方的棋子
			}
			else if (i < 0)
			{
				gotoxy(0, H); printf("遠端離線");
				break;
			}
		}
	}
}
void CFun(PVOID p)
{
	sockaddr addr;
	char s1[2000];
	int x, y, i, Len = sizeof(sockaddr);
	while (1)
	{
		i = recv(CSock, s1, sizeof(s1) - 1, 0);
		if (i > 0)
		{
			s1[i] = 0;
			i = sscanf_s(s1, "(%d-%d", &x, &y);
			if (i == 2) 
			{   
				gotoxy(x, y); 
			    printf("X");
				T[x][y] = 'X';
				check();
            }
        }
		else if (i < 0)
		{
			gotoxy(0, H); printf("遠端離線"); 
			break;
		}
	}
}

int main()
{
	int x, y, z,x1,y1;
	char IP[100], S1[200];
	
    //1.繪製遊戲框
	for (y = 0; y < H; y++)
		for (x = 0; x < W; x++)
		{
			    T[x][y] = 0;
			    gotoxy(x, y); x1 = x % 2; y1 = y % 2;
				if ((x1) && (y1))printf("+");
				else if ((!x1) && (y1))printf("-");
				else if ((x1) && (!y1))printf("|");
		}

	//2.啟動tcp server(等待遠端連入,Thread)
	Start_TCP_Server(&SSock, 6000);
	_beginthread(SFun, 0, 0);// 處理接待客人連入

	//3.讀取鍵盤，處理:連線、下棋、分析、顯示....
	unsigned char c;
	int cx = W / 2, cy = H / 2;//遊戲初始位置(中央)
	do
	{
		gotoxy(cx, cy);
		c = _getch();
		//1.移動游標
		if (c == 0x48) cy -= 2;//上
		else if (c == 0x50) cy += 2;//down
		else if  (c == 0x4b) cx -= 2;//left 
		else if  (c == 0x4d) cx += 2;//right
		if (cx < 0) cx = 0; else if (cx >= W) cx = W - 2;
		if (cy < 0) cy = 0; else if (cy >= H) cy = H - 2;
		//2.下棋
		if (c == 0x0d)
		{
			printf("O"); //Enter
			T[cx][cy] = 'O';
			sprintf_s(S1, "(%d-%d)", cx, cy);
			send(Sock, S1, strlen(S1) - 1, 0);
		}
		//3.找人玩
		else if ((c == 'c') || (c == 'C'))
		{
			gotoxy(0, H);
			printf("IP=");
			scanf_s("%s", IP, 99);
			Start_TCP_Client(&CSock, 6000, IP);
			F = 1;
			_beginthread(CFun, 0, 0);
			Sock = CSock;
		}
		//4.分析: 五連線、活三、絕四
            //預測: 雙活三點、雙絕四點、活三絕四點
		check();//五連線

	} 
	while ((c != 'q') && (c != 'Q'));
	gotoxy(0, H); 
	printf("遊戲結束");
	closesocket(SSock);
	if (F >= 1)
		closesocket(Sock);
		
}

