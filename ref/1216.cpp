// 1216.cpp : 此檔案包含 'main' 函式。程式會於該處開始執行及結束執行。
//

#include <iostream>
#include<conio.h> //for _getch()
#include "TCPIP_Sync.h" //for star_TCP_client
//連線FTP參數
//char IP[100] = { "195.144.107.198" };//FTPS Server IP  demo/password
char IP[100] = { "210.61.132.2" };// ftp/ftp
int Port = 21;  //FTP 連線通訊
				//FTP連線代碼
SOCKET CSock;

int RETR_F = 0, RETR_Port = 0;
char RETR_IP[100], RETR_File[100];
SOCKET RETR_Sock;

void RETR_Fun(PVOID p) 
{  
	//下載檔案
	char S1[2000];
	int i;
	errno_t err;
	FILE *Out;
	if ((err = fopen_s(&Out, RETR_File, "wb")) != 0) {
		printf("File %s error!\n", RETR_File); return;
	}
	while (1) {
		i = recv(RETR_Sock, S1, sizeof(S1) - 1, 0);
		if (i > 0) fwrite(S1, 1, i, Out);//Binary寫檔
		else break;
	}
	fclose(Out);
}

int PASV_F = 0;
//for NLST (Download Directory Contents)
int NLST_F = 0, NLST_Port = 0;
char NLST_IP[100];
SOCKET NLST_Sock;

void NLST_Fun(PVOID p) {
	char S1[2000];
	int i;
	while (1) {
		i = recv(NLST_Sock, S1, sizeof(S1) - 1, 0);
		if (i > 0) {
			S1[i] = 0; printf("%s\r\n", S1);
		}
		else break;
	}
}

void Get_IP_Port(char *S1, char *IP, int *Port) {
	int i = 0, IPn[10];
	while (S1[i] != '(') i++;
	sscanf_s(&S1[i], "(%d,%d,%d,%d,%d,%d", &IPn[0], &IPn[1], &IPn[2], &IPn[3], &IPn[4], &IPn[5]);
	sprintf_s(IP, 30, "%d.%d.%d.%d", IPn[0], IPn[1], IPn[2], IPn[3]);
	*Port = IPn[4] * 256 + IPn[5];
}

void sFTP(PVOID p) { //FTP執行續(負責接收資料)
	char S1[2000], IP1[100];
	int i, Port1 = 0;
	while (1) {
		i = recv(CSock, S1, sizeof(S1) - 1, 0);
		if (i > 0) {
			S1[i] = 0;
			//處理 下載目錄資料指令(NLST)
			//NLST: PASV+Get_IP_Port+Start_TCP_C=send("NLST")=recv("directory")
			if (NLST_F) {
				Get_IP_Port(S1, NLST_IP, &NLST_Port); NLST_F = 0;
				Start_TCP_Client(&NLST_Sock, NLST_Port, NLST_IP);
				_beginthread(NLST_Fun, 0, 0);
				send(CSock, "NLST\r\n", 6, 0);
			}
			else if (RETR_F) {
				Get_IP_Port(S1, RETR_IP, &RETR_Port); RETR_F = 0;
				Start_TCP_Client(&RETR_Sock, RETR_Port, RETR_IP);
				_beginthread(RETR_Fun, 0, 0);
				sprintf_s(S1, "RETR %s\r\n", RETR_File);
				send(CSock, S1, strlen(S1), 0);
			}
			//一般收發命令
			else printf("%s\r\n", S1);
		}

	}
}

int main()
{
	int C;
	system("chcp 950");
	char S1[2000];
	//1.連線TCP Server
	Start_TCP_Client(&CSock, Port, IP);
	_beginthread(sFTP, 0, 0);
	Sleep(1000);
	//2.輸入帳號
	char Name[100];
	printf("FTP Account="); scanf_s("%s", Name, 99);
	sprintf_s(S1, "USER %s\r\n", Name);
	send(CSock, S1, strlen(S1), 0);
	Sleep(1000);//休息一秒
	//3.輸入密碼
	char Password[100] = {};
	printf("FTP Password=");
	C = 0;
	do {
		Password[C++] = _getch();
		printf("*");
	} while (Password[C - 1] != 0x0d);
	Password[C - 1] = 0;
	sprintf_s(S1, "PASS %s\r\n", Password);
	send(CSock, S1, strlen(S1), 0);
	Sleep(1000);//休息一秒
	//4.讀取鍵盤，送出FTP操作命令
	char Cmd[1000];
	while (strcmp(Cmd, "QUIT") && strcmp(Cmd, "quit")) {
		C = 0;
		printf("FTP>"); //scanf_s("%s",Cmd,99);
		do {
			Cmd[C++] = _getche();
		} while (Cmd[C - 1] != 0x0d);//當未按下enter
		Cmd[C - 1] = 0; printf("\r\n");

		//Download Directory/ NLST => PASV + Get IP_Port + Start_TCP_Client + NLST+rec...
		if (!strcmp(Cmd, "NLST") || (!strcmp(Cmd, "nlst"))) {
			strcpy_s(Cmd, 99, "PASV"); NLST_F = 1;
		}
		//Download File/ RETR => PASV + Get IP_Port + Start_TCP_Client + NLST+rec...
		if (!strncmp(Cmd, "RETR", 4)) {
			sscanf_s(Cmd, "RETR %s", RETR_File, 99);
			strcpy_s(Cmd, 99, "PASV"); RETR_F = 1;
		}
		if (!strncmp(Cmd, "retr", 4)) {
			sscanf_s(Cmd, "retr %s", RETR_File, 99);
			strcpy_s(Cmd, 99, "PASV"); RETR_F = 1;
		}

		sprintf_s(S1, "%s\r\n", Cmd);
		send(CSock, S1, strlen(S1), 0);
		Sleep(1000);
	}
	printf("結束FTP程式，再見");

}
