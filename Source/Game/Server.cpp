#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "Server.h"
#include <iostream>
#include <ws2tcpip.h>
#include <stdio.h>
#include <algorithm>
#include "SharedDefines.h"
#pragma comment(lib, "Ws2_32.lib")

#include "Messages/CNetMessageVector2i.h"
#include "Messages/CNetMessageChatMessage.h"
#include "Client.cpp"
#include "Game.h"

void MakeLowerCase(std::string &string)
{
	std::transform(string.begin(), string.end(), string.begin(), [](unsigned char c) { return std::tolower(c); });
}

void Server::Init()
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, 10);

	WSADATA wsaData;

	int iResult = 0;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed: %i\n", iResult);
		return;
	}

	// Set adress info
	struct addrinfo *result = NULL, hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(__argv[1], DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed: %i\n", iResult);
		WSACleanup();
		return;
	}

	//system("adb forward udp:8081 udp:8082");

	// Create socket
	myClientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (myClientSocket == INVALID_SOCKET)
	{
		printf("error at socket():  %i\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	}

	printf("Enter your internal ip: ");
	std::string ip = DEFAULT_SERVER_IP_INTERNAL;
	if (std::cin.peek() != '\n')
		std::cin >> ip;
	std::cin.ignore();

	SOCKADDR_IN receiverAddr;
	receiverAddr.sin_family = AF_INET;
	receiverAddr.sin_port = htons(DEFAULT_PORT_NUMBER);
	receiverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

	// Bind socket
	iResult = bind(myClientSocket, (SOCKADDR *)&receiverAddr, sizeof(receiverAddr));
	if (iResult == SOCKET_ERROR)
	{
		printf("bind failed with error: %i\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(myClientSocket);
		WSACleanup();
		return;
	}
	freeaddrinfo(result);

	std::cout << "Server started on " << ip << "\n";
	Listen();
}

void Server::StartGame()
{
	CGame myGame;
	myGame.myServer = this;
	myGame.Init(L"1337", true); // Blocking
}

void Server::StartListenThread()
{
	std::thread listenThread(&Server::Listen, this);
	listenThread.detach();
}

void Server::Listen()
{
	// Set up time interval
	timeval timeout;
	timeout.tv_sec = 1000;
	timeout.tv_usec = 0;
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(myClientSocket, &fds);
	int selectTiming = select(0, &fds, 0, 0, &timeout);

	char receiveBuf[DEFAULT_BUFLEN];
	int bytesReceived = 0;
	SOCKADDR_IN senderAddr;
	int senderAddrSize = sizeof(senderAddr);
	char IDmaker = 0;

	std::cout << "Server listening...\n";

	switch (selectTiming)
	{
	case 0:
		printf("Timeout lor while waiting... -.-\n");
		break;
	case -1:
		printf("Error: %ld\n", WSAGetLastError());
		break;
	default:
		while (1337)
		{
			memset(receiveBuf, '\0', DEFAULT_BUFLEN);
			bytesReceived = recvfrom(myClientSocket, receiveBuf, DEFAULT_BUFLEN, 0, (SOCKADDR *)&senderAddr, &senderAddrSize);
			if (bytesReceived > 0)
			{
				char *dataFromNet = receiveBuf;
				char netBufIn[DEFAULT_BUFLEN];
				memcpy(&netBufIn, dataFromNet, DEFAULT_BUFLEN);
				
				if ((ClassTypeID)netBufIn[0] == ClassTypeID::StringMessage)
				{
					char receiveBuf[DEFAULT_BUFLEN];
					int senderAddrSize = sizeof(senderAddr);
					static char IDmaker = 0;

					CNetMessageChatMessage newMessage;
					memcpy(&newMessage, &netBufIn, DEFAULT_BUFLEN);

					HandleStringMessage(newMessage.GetText(), senderAddr);
				}
				else if ((ClassTypeID)netBufIn[0] == ClassTypeID::Vector2iMessage)
				{
					//CNetMessageVector2i newMessage;
					//memcpy(&newMessage, &netBufIn, DEFAULT_BUFLEN);

					for (auto &user : myUsers)
					{
						if ((senderAddr.sin_addr.S_un.S_addr != user.myAddr.sin_addr.S_un.S_addr) || (senderAddr.sin_port != user.myAddr.sin_port))
						{
							int iSendResult = sendto(myClientSocket, netBufIn, DEFAULT_BUFLEN, 0, (SOCKADDR *)&user.myAddr, senderAddrSize);
							if (iSendResult == SOCKET_ERROR)
							{
								printf("send failed: %d\n", WSAGetLastError());
								closesocket(myClientSocket);
								WSACleanup();
								return;
							}
						}
					}
				}
			}
			else if (bytesReceived <= 0)
			{
				printf("Server: Connection closed with error code : % ld\n", WSAGetLastError());
			}
		}
	}
}

void Server::HandleStringMessage(const std::string &aData, SOCKADDR_IN &senderAddr)
{
	static char IDmaker = 0;
	char netBufIn[DEFAULT_BUFLEN];
	int senderAddrSize = sizeof(senderAddr);

	if (aData != "handshake" || myUsers.size() > 2)
	{
		return;
	}

	CNetMessageChatMessage newMessage;
	bool isWhite = myUsers.size() >= 1 ? !myUsers[0].myIsWhite : rand() % 2 == 0;
	newMessage.SetMessage(isWhite ? "white" : "black");
	newMessage.myID = IDmaker++;

	myUsers.push_back({ senderAddr, 0 });
	myUsers.back().myIsWhite = isWhite;
	newMessage.myGameHasStarted = myUsers.size() == 2;

	memcpy(netBufIn, &newMessage, sizeof(CNetMessageChatMessage));

	int iSendResult = sendto(myClientSocket, netBufIn, DEFAULT_BUFLEN, 0, (SOCKADDR *)&senderAddr, senderAddrSize);
	if (iSendResult == SOCKET_ERROR)
	{
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(myClientSocket);
		WSACleanup();
		return;
	}
}