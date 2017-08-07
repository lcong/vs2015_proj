/*����������ķ�ʽ����
  �����ʽ���£�
  client [-p:x] [-s:IP] [-n:x] [-o]
  -p:x     Զ�̷������˿ں�
  -s:IP    Զ�̷�����IP
  -n:x     ��Ϣ���͵Ĵ���
  -o       ֻ������Ϣ�������ջ�Ӧ��Ϣ
*/
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT_COUNT 20 /*��Ϣ���͵�Ĭ�ϴ���*/ 
#define DEFAULT_PORT 5150 /*Ĭ�϶˿ں�*/
#define DEFAULT_BUFFER 2048 /*��������С*/
#define DEFAULT_MESSAGE "This is a socket sample" /*���͵���Ϣ����*/

char szServer[128], szMessage[1024];
unsigned short iPort = DEFAULT_PORT;
DWORD dwCount = DEFAULT_COUNT;
BOOL bSendOnly = FALSE;

/*��ʾ�����б�*/
void usage()
{
	printf("usage: client [-p:x] [-s:IP] [-n:x] [-o]\n\n");
	printf("       -p:x     Remote port to send to \n");
	printf("       -s:IP    Server's IP address or host name\n");
	printf("       -n:x     Number of times to send message\n");
	printf("       -o       Send messages only: don't receive\n");
	ExitProcess(1);
}

/*��֤����*/
void ValidateArgs(int argc, char *argv[])
{
	int i;
	for (i = 0; i < argc; i++)
	{
		if ((argv[i][0] == '-') || (argv[i][0] == '/'))
		{
			switch (tolower(argv[i][1]))
			{
				/*������֮ͨ�ŵķ������˿�*/
				case 'p':
					if (strlen(argv[i]) > 3)
						iPort = atoi (&argv[i][3]);
					break;

				/*������֮ͨ�ŵķ�����IP*/
				case 's':
					if (strlen(argv[i]) > 3)
						strcpy(szServer, &argv[i][3]);
					break;

				/*������Ϣ�ķ��ʹ���*/
				case 'n':
					if (strlen(argv[i]) > 3)
						dwCount = atol(&argv[i][3]);
					break;

				/*ֻ������Ϣ�������ջ�Ӧ��Ϣ*/
				case 'o':
					bSendOnly = TRUE;
					break;
				/*�����Ƿ�����ʾ�����б�*/	
				default:
					usage();
					break;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	WSADATA wsd;
	SOCKET sClient;
	char szBuffer[DEFAULT_BUFFER];
	int ret;
	struct sockaddr_in server;
	struct hostent *host = NULL;
	DWORD i;
	
	/*��֤����*/
	ValidateArgs(argc, argv);
	/*����Winsock DLL*/
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		printf("Failed to load Winsock library!\n");
		return 1;
	}
	strcpy(szMessage, DEFAULT_MESSAGE);
	
	/*����Socket*/
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == INVALID_SOCKET)
	{
		printf("socket() failed: %d\n", WSAGetLastError());
		return 1;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(iPort);
	server.sin_addr.s_addr = inet_addr(szServer);

	if (server.sin_addr.s_addr == INADDR_NONE)
	{
		host = gethostbyname(szServer);
		if (host == NULL)
		{
			printf("Unable to resolve server: %s\n", szServer);
			return 1;
		}
		CopyMemory(&server.sin_addr, 
				   host->h_addr_list[0],
			       host->h_length);
	}
	/*���������������*/
	if (connect(sClient, 
		        (struct sockaddr*)&server,
			    sizeof(server)) 
		== SOCKET_ERROR)
	{
		printf("connect() failed: %d\n", WSAGetLastError());
		return 1;
	}

	/*���͡�������Ϣ*/
	for (i = 0; i < dwCount; i++)
	{
		ret = send(sClient, szMessage, strlen(szMessage), 0);
		if (ret ==0 )
		{
			break;
		}
		else if (ret == SOCKET_ERROR)
		{
			printf("send() failed: %d\n", WSAGetLastError());
			break;
		}
		printf("Send %d bytes\n", ret);
		if (!bSendOnly)
		{
			ret = recv(sClient, szBuffer, DEFAULT_BUFFER, 0);
			if (ret == 0)
			{
				break;
			}
			else if (ret == SOCKET_ERROR)
			{
				printf("recv() failed: %d\n", WSAGetLastError());
				break;
			}
			szBuffer[ret] = '\0';
			printf("RECV [%d bytes]: %s\n", ret, szBuffer);
		}
	}
	closesocket(sClient);
	WSACleanup();
	return 0;
}