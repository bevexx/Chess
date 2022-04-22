#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "Client.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <sstream>
#include <thread>

#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

#include <Messages/CNetMessageChatMessage.h>
#include <Messages/CNetMessageVector2i.h>
#include "GameWorld.h"
#include <tga2d/sprite/sprite.h>

#include <tga2d/drawers/debug_drawer.h>
#include <tga2d/error/error_manager.h>

void Client::Init()
{
	//SetConsoleCtrlHandler((PHANDLER_ROUTINE)OnClose, TRUE);

	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, 12);

	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	// Create socket
	myServerSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (myServerSocket == INVALID_SOCKET)
	{
		printf("error at socket(): %i\n", WSAGetLastError());
		//freeaddrinfo(result);
		WSACleanup();
		return;
	}

	printf("Enter the ip you want to connect to: ");
	std::string ip = DEFAULT_SERVER_IP_EXTERNAL;
	if (std::cin.peek() != '\n')
		std::cin >> ip;
	std::cin.ignore();

	myWaiting = true;
	std::thread waitThread(&Client::SendAndWaitForResponse, this, ip);
	waitThread.detach();
}

void Client::Start()
{
	if (myWaiting)
	{
		myListenQueued = true;
		return;
	}

	StartProfileThread();
	std::thread listenThread(&Client::Listen, this);
	listenThread.detach();
}

void Client::SendBytes(char aBuffer[DEFAULT_BUFLEN])
{
	myBytesSent += DEFAULT_BUFLEN;
	int iResult = send(myServerSocket, aBuffer, DEFAULT_BUFLEN, 0);
	if (iResult == SOCKET_ERROR)
	{
		printf("send failed: %i\n", WSAGetLastError());
	}
}

void Client::StartProfileThread()
{
	std::thread listenThread(&Client::Profile, this);
	listenThread.detach();
}

void Client::Profile() 
{
	int lastBytesSent = 0;
	int lastBytesGot = 0;
	while (true)
	{
		float time = ((float)std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - startTime).count() / 1000.0f);
		if (time > 100.0f)
		{
			system("cls");
			std::cout << "B/s sent: " << (float)(myBytesSent - lastBytesSent) / time << "\n";
			std::cout << "B/s recieved: " << (float)(myBytesGot - lastBytesGot) / time << "\n";
			std::cout << "Ping: " << myPing << "ms \n";
			std::cout << "Loss: " << (myServerBytesSent > 0 ? (1.0f - (float)myBytesGot / (float)myServerBytesSent) * 100.0f : 0.0f) << "% \n";
			startTime = std::chrono::system_clock::now();
			lastBytesSent = myBytesSent;
			lastBytesGot = myBytesGot;
		}
	}
}

void Client::Listen()
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	int iResult = 0;
	char buffer[DEFAULT_BUFLEN];
	SOCKADDR_IN remoteAddr;
	int         remoteAddrLen = sizeof(SOCKADDR_IN);
	do
	{
		iResult = recvfrom(myServerSocket, buffer, DEFAULT_BUFLEN, 0, (SOCKADDR*)&remoteAddr, &remoteAddrLen);

		char *dataFromNet = buffer;
		char netBufIn[DEFAULT_BUFLEN];
		const int sizeOfBytesGot = DEFAULT_BUFLEN;
		memcpy(&netBufIn, dataFromNet, sizeOfBytesGot);

		if ((ClassTypeID)netBufIn[0] == ClassTypeID::Vector2iMessage)
		{
			CNetMessageVector2i newMessage;
			memcpy(&newMessage, &netBufIn, sizeof(CNetMessageVector2i));

			std::pair<Vector2i, Vector2i> moves = newMessage.Get2Vector2i();
			myGameWorld->MakeMove({ moves.first.x, moves.first.y }, { moves.second.x, moves.second.y });
		}

	} while (iResult > 0);
}

void Client::SendAndWaitForResponse(const std::string &aIP)
{
	CNetMessageChatMessage message;
	message.SetMessage("handshake");
	memcpy(myWriteBuffer, &message, sizeof(CNetMessageChatMessage));

	char buffer[DEFAULT_BUFLEN];
	SOCKADDR_IN remoteAddr;
	int remoteAddrLen = sizeof(SOCKADDR_IN);

	addrinfo *ptr = NULL;
	while (myID == -1)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		struct addrinfo *result = NULL, hints;
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;

		getaddrinfo(__argv[1], "25565", &hints, &result);
		ptr = result;
		
		SOCKADDR_IN receiverAddr;
		receiverAddr.sin_family = AF_INET;
		receiverAddr.sin_port = htons(25565);
		receiverAddr.sin_addr.s_addr = inet_addr(aIP.c_str());

		if (connect(myServerSocket, (SOCKADDR *)&receiverAddr, sizeof(receiverAddr)))
		{
			continue;
		}

		SendBytes(myWriteBuffer);

		Sleep(50);

		int bytes = recvfrom(myServerSocket, buffer, DEFAULT_BUFLEN, 0, (SOCKADDR *)&remoteAddr, &remoteAddrLen);
		if (bytes > 0)
		{
			char *dataFromNet = buffer;
			char netBufIn[DEFAULT_BUFLEN];
			memcpy(&netBufIn, dataFromNet, DEFAULT_BUFLEN);

			if ((ClassTypeID)netBufIn[0] == ClassTypeID::StringMessage)
			{
				CNetMessageChatMessage newMessage;
				memcpy(&newMessage, &netBufIn, DEFAULT_BUFLEN);

				std::string data(newMessage.GetText());
				if (data == "white")
				{
					myGameWorld->myBoard.myCheckmate = false;
					myID = newMessage.myID;
				}
				else if (data == "black")
				{
					myGameWorld->myBoard.myCheckmate = false;
					myGameWorld->FlipBoard();
					myGameWorld->myClientIsWhite = false;
					myID = newMessage.myID;
				}
			}
		}
	}

	freeaddrinfo(ptr);

	myWaiting = false;
	if (myListenQueued)
	{
		myListenQueued = false;
		StartProfileThread();
		Listen();
	}
}