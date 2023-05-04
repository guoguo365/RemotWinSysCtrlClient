#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>

#include "cmd.h"

#pragma comment(lib, "ws2_32.lib")

SOCKET clientSocket;

int main() {

	// �Ƿ�����
	isReBootProc();

	// 1. ȷ��Э��汾��Ϣ
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("ȷ��Э��汾��Ϣ����%d\n", GetLastError());
		return -1;
	}
	printf("�ͻ���-ȷ��Э��汾��Ϣ�ɹ���\n");

	// 2. ����socket 
	clientSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET:IPV4, SOCK_STREAM:TCP, SOCK_DGRAM
	if (SOCKET_ERROR == clientSocket) {
		printf("�ͻ���-����socketʧ�ܣ�%d\n", GetLastError());
		return -1;
	}
	printf("�ͻ���-����socket�ɹ���\n");

	// 3. ȷ��������Э���ַ��
	SOCKADDR_IN sAddr;
	sAddr.sin_family = AF_INET; // socket������һ������
	sAddr.sin_addr.S_un.S_addr = inet_addr("10.4.30.167");
	sAddr.sin_port = htons(10086); 

	// 4. ���ӷ�����
	int c = connect(clientSocket, (sockaddr*) &sAddr, sizeof(sAddr));
	if (SOCKET_ERROR == c) {

		// 8. �ر�socket
		closesocket(clientSocket);

		// 9. ����Э��汾��Ϣ
		WSACleanup();
		printf("�ͻ���-���ӷ�����ʧ��: %d\n", GetLastError());
		return -1;
	}
	printf("�ͻ���-���ӷ������˳ɹ���\n");

	// �ȴ�������Ϣ
	while (true)
	{
		char recvBuff[256];
		int r = recv(clientSocket , recvBuff, 255, NULL);
		if (r > 0) {

			// ��ӽ����� \0
			recvBuff[r] = 0;
			printf("����˷�������:%s\n", recvBuff);

			/*
			1.ע�� 2.���� 3.�ػ� 4.��������
			*/
			if (strcmp("1", recvBuff) == 0) {
				printf("����ע��...\n");
				if (logOffProc()) {
					printf("ע���ɹ���\n");
				}
				else {
					printf("ע��ʧ�ܣ�\n");
				}
				
			}
			else if (strcmp("2", recvBuff) == 0) {
				printf("��������...\n");
				reBootProc();
			}
			else if (strcmp("3", recvBuff) == 0) {
				printf("���ڹػ�...\n");
				powerOffProc();
			}
			else if (strcmp("4", recvBuff) == 0) {
				printf("������������...\n");
				infiniteReBootProc();
			}
		}
	}

	// 6. �ر�socket
	closesocket(clientSocket);

	// 7. ����Э��汾��Ϣ
	WSACleanup();

	return 0;
}