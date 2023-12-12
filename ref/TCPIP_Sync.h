#pragma once

// For TCPIP
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1  // ¨t²Î­n¨D
#pragma comment(lib,"ws2_32.lib")
#include <winsock2.h>

int Start_UDP_Server(SOCKET *SSock,int Port)
{
	// 1.ÅÜ¼Æ«Å§i
	WSADATA Wsa;      // Winsock °Ñ¼Æ
	//SOCKET  SSock;    // ³q¹D³s½u¥N½X
	sockaddr_in Addr; // IP+Port+Protocol

	// 2.³]©wWinsock/sock
	WSAStartup(0x202, &Wsa); // ±Ò°ÊWinsock
	*SSock = socket(AF_INET, SOCK_DGRAM, 0); // ¶}±ÒUDP³q¹D
	Addr.sin_family = AF_INET;
	Addr.sin_port = htons(6000);
	Addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// 3.±Ò°ÊUDP Server
	bind(*SSock, (sockaddr *)&Addr, sizeof(sockaddr));
	return 0;
}

int Start_UDP_Client(SOCKET *CSock, sockaddr *CAddr, char *IP,int Port)
{
	// 1.ÅÜ¼Æ«Å§i
	WSADATA Wsa;      // Winsock °Ñ¼Æ
	//SOCKET  SSock;    // ³q¹D³s½u¥N½X
	sockaddr_in *Addr; // IP+Port+Protocol
	
	Addr = (sockaddr_in *)CAddr;
	// 2.³]©wWinsock/sock
	WSAStartup(0x202, &Wsa); // ±Ò°ÊWinsock
	*CSock = socket(AF_INET, SOCK_DGRAM, 0); // ¶}±ÒUDP³q¹D
	Addr->sin_family = AF_INET;
	Addr->sin_port = htons(6000);
	Addr->sin_addr.s_addr = inet_addr(IP);

	return 0;
}

// =======================================================================
// ==================  ±Ò°Ê TCP Server(¨Ï¥Îthread±µ¨ü³s½u»P±µ¦¬¸ê®Æ)   ===
// =======================================================================
#include <process.h>  // Project => Setting => C/C++ => Category=Code Generation => Use Runtime Library=Debug Multithreaded
SOCKET   g_S_Socket, g_S_Socket1;
DWORD    Thread_ID11;
// °õ¦æºü¤lµ{¦¡
VOID Thread_Func11(PVOID pvoid)
{
	char S1[5000];
	int  i, Len = sizeof(sockaddr);
	sockaddr_in Addr;

	while (1)
	{
		g_S_Socket1 = accept(g_S_Socket, (sockaddr *)&Addr, &Len);
		do
		{
			i = recv(g_S_Socket1, S1, sizeof(S1) - 1, 0);
			if (i > 0) {
				S1[i] = 0;
				printf("Recv(%s) = %s\r\n", inet_ntoa(Addr.sin_addr), S1);
			}
		} while (i > 0);
	}
}
int  Start_TCP_Server(SOCKET *psockfd, WORD Port)
{
	WSADATA  wsadata;
	int      err;
	struct   sockaddr_in  tcpserver;

	// 1. ¶}±Ò TCP Server
	if ((err = WSAStartup(0x202, &wsadata)) != 0) return -1;
	if ((*psockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;
	tcpserver.sin_family = AF_INET;
	tcpserver.sin_port = htons(Port);
	tcpserver.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(*psockfd, (struct sockaddr *)&tcpserver, sizeof(tcpserver)) < 0) return -1;
	if ((err = listen(*psockfd, SOMAXCONN)) < 0) return -1;
	g_S_Socket = *psockfd;

	// 2. ¶}±Ò¤l°õ¦æºü
	Thread_ID11 = _beginthread(Thread_Func11, 0, NULL);

	return 0;
}

// =======================================================================
// ==================  ±Ò°Ê TCP Client   =================================
// =======================================================================
SOCKET   g_C_Socket; // Global Variable
DWORD    Thread_ID22;
// °õ¦æºü¤lµ{¦¡
VOID Thread_Func22(PVOID pvoid)
{
	char S1[2000];
	int  i, j, Len = sizeof(sockaddr);
	sockaddr_in Addr;
	BOOL Flag = 0;
	int  k, k1, k2, k3, Beg, End, Fa2 = 0;
	char S2[2000];
	while (1)
	{
		i = recv(g_C_Socket, S1, sizeof(S1) - 1, 0);
		if (i > 0)
		{
			S1[i] = 0;
			// ¥h°£ ÃC¦â±±¨î½X(Remove Color Control Code) 
			for (j = 0; j < i; j++)
			{
				if (S1[j] == 27) { Flag = true; Beg = j + 1; } // ESC
				if (!Flag) printf("%c", S1[j]);
				if ((Flag) && (S1[j] == 'm'))
				{
					Flag = false; End = j;
					memcpy(S2, &S1[Beg], End - Beg + 1);
					S2[End - Beg + 1] = 0;
					k = sscanf_s(S2, "[%d;%d;%dm", &k1, &k2, &k3);
					//if ((k==1)&&(k1==41))
				}
			}
			//printf("%s",S1);
			//for (i=0;i<strlen(S1);i++)
			//{   if      (S1[i]==0x1b) Flag=1;
			//    else if (S1[i]=='m')  Flag=0;
			//    else if (Flag==0) printf("%c",S1[i]);
		} //}   
	}
}
int Start_TCP_Client(SOCKET *psockfd, WORD R_Port, char *IP)
{
	WSADATA  wsadata;
	int      err;
	sockaddr_in  tcpclient;

	if ((err = WSAStartup(0x202, &wsadata)) != 0) return -1;
	if ((*psockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) return -1;
	tcpclient.sin_family = AF_INET;
	tcpclient.sin_port = htons(R_Port);
	tcpclient.sin_addr.s_addr = inet_addr(IP);
	if (err = connect(*psockfd, (sockaddr *)&tcpclient, sizeof(tcpclient)) < 0) return -1;
	g_C_Socket = *psockfd;

	// 2. ¶}±Ò¤l°õ¦æºü
	//Thread_ID22 = _beginthread(Thread_Func22, 0, NULL);


	return 0;
}
