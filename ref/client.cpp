// client.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//


#include <iostream>
#include"TCPIP_Sync.h"
SOCKET Sock;
void Fun(PVOID p)
{
	char S1[2000];
	int i;
	while (1)
	{
		i = recv(Sock, S1, sizeof(S1) - 1, 0);
		if (i < 0) {
			S1[i] = 0;
			printf("%s\n", S1);
		}
	}
}

int main()
{   //練習一:連線陽明交大網頁

 //char IP[100] = { "64.185.228.67 " };
 //char IP[100] = { "140.1133.9.151 " };
 //char IP[100] = { "140.113.43.27 " };
	char IP[100] = { "127.0.0.1" };
	int  Port = 6000;
	system("chcp 950");

	//以tcp連線伺服器
	Start_TCP_Client(&Sock, Port, IP);

	//啟動thread接收資料
	_beginthread(Fun, 0, 0);

	//讀鍵盤，送資料
	char S1[2000];
	while (1)
	{
		printf(">");
		scanf_s("%s", S1, 1999);
		send(Sock, S1, strlen(S1), 0);
	}
}

