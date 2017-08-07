/*����������ķ�ʽ����
  �����ʽ���£�
  server [-p:x] [-i:IP] [-o]
  -p:x     ���м����Ķ˿ں�
  -i:str   ���м�����IP,Ӧ���ڶ�ip������
  -o       ֻ������Ϣ�������ͻ�Ӧ��Ϣ
*/
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_PORT 5150 /*Ĭ�϶˿ں�*/
#define DEFAULT_BUFFER 4896 /*��������С*/

unsigned short iPort = DEFAULT_PORT;
BOOL bInterface = FALSE, bRecvOnly = FALSE;
char szAddress[128];

/*��ʾ�����б�*/
void usage()
{
	printf("usage: server [-p:x] [-i:IP] [-o]\n\n");
	printf("       -p:x      Port number to listen on\n");
	printf("       -i:str    Interface to listen on\n");
	printf("       -o        Don't echo the data back\n\n");
	ExitProcess(1);
}

/*��֤����*/
void ValidateArgs(int argc, char **argv)
{
	int i;

	for (i = 1; i < argc; i++)
	{
		if ((argv[i][0] =='-') || (argv[i][0] == '/'))
		{
			switch (tolower(argv[i][1]))
			{
			/*ָ���˿�*/
			case 'p':
				iPort = atoi(&argv[i][3]);
				break;

			/*ָ��IP*/
			case 'i':
				bInterface = TRUE;
				if (strlen(argv[i]) > 3)
					strcpy(szAddress, &argv[i][3]);
				break;

			/*ֻ������Ϣ������Ӧ*/
			case 'o':
				bRecvOnly = TRUE;
				break;

			/*�Ƿ���������ʾ�����б�*/
			default:
				usage();
				break;
			}
		}
	}
}

/*��ͻ���ͨ�ŵ��̺߳���*/
DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET sock = (SOCKET)lpParam;
	char szBuff[DEFAULT_BUFFER];
	int ret, nLeft, idx;
	while (1)
	{
		/*�������Կͻ�������Ϣ*/
		ret = recv(sock, szBuff,DEFAULT_BUFFER, 0);
		if (ret == 0)
			break;
		else if (ret == SOCKET_ERROR)
		{
			printf("recv() failed: %d\n", WSAGetLastError());
			break;
		}
		szBuff[ret] = '\0';
		printf("RECV: %s\n", szBuff);

		if (!bRecvOnly)
		{
			nLeft = ret;
			idx = 0;
			while (nLeft > 0)
			{
				/*��ͻ������ͻ�Ӧ��Ϣ*/
				ret = send(sock, &szBuff[idx], nLeft, 0);
				if (ret == 0)
					break;
				else if (ret == SOCKET_ERROR)
				{
					printf("send() failed: %d\n", WSAGetLastError());
					break;
				}
				nLeft -=ret;
				idx +=ret;
			}
		}

	}
	return 0;
}

int main(int argc, char **argv)
{
	WSADATA wsd;
	SOCKET sListen, sClient;
	int iAddrSize;
	HANDLE hThread;
	DWORD dwThread;
	struct sockaddr_in local, client;
	
	/*��֤����*/
	ValidateArgs(argc, argv);
	
	/*����Winsock DLL*/
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("Failed to load WinSock!\n");
		return 1;
	}

	/*����Socket*/
	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sListen == SOCKET_ERROR)
	{
		printf("socket() failed: %d\n", WSAGetLastError());
		return 1;
	}
	if (bInterface)
	{
		local.sin_addr.s_addr = inet_addr(szAddress);
		if (local.sin_addr.s_addr == INADDR_NONE)
		{
			usage();
		}
	}
	else 
	{
		local.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	local.sin_family = AF_INET;
	local.sin_port = htons(iPort);

	/*��Socket*/
	if (bind(sListen, 
		     (struct sockaddr*)&local,
			 sizeof(local)) == SOCKET_ERROR)
	{
		printf("bind() failed: %d\n", WSAGetLastError());
		return 1;
	}
	/*�򿪼���*/
	listen(sListen, 8);

	/*�ڶ˿ڽ��м�����һ���пͻ�������������ʾ
	�ͽ�����ͻ�������ͨ�ŵ��߳�*/
	while(1)
	{
		iAddrSize = sizeof(client);
		/*�����Ƿ�����������*/
		sClient = accept(sListen, 
						 (struct sockaddr*)&client, 
						 &iAddrSize);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept() failed: %d\n",WSAGetLastError());
			break;
		}
		printf("Acceped client: %s:%d\n",
			   inet_ntoa(client.sin_addr), 
			   ntohs(client.sin_port));

		hThread = CreateThread(NULL, 
							   0, 
							   ClientThread,
					           (LPVOID)sClient, 
							   0, 
							   &dwThread);
		if (hThread == NULL)
		{
			printf("CreateThread() failed: %d\n", GetLastError());
			break;
		}
		CloseHandle(hThread);
	}
	closesocket(sListen);
	WSACleanup();
	return 0;
}