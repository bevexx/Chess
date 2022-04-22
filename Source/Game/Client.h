#pragma once
#include <WinSock2.h>
#include "SharedDefines.h"
#include <chrono>
#include <string>

class CGameWorld;

//BOOL WINAPI OnClose();
//static SOCKET ourExitSocket = INVALID_SOCKET;
//static char ourID = -1;

class Client
{
public:
	void Init();
	void Start();
	void Listen();
	void SendBytes(char aBuffer[DEFAULT_BUFLEN]);
	
	void StartProfileThread();
	void Profile();

	CGameWorld *myGameWorld;
	char myID = -1;

	int myBytesSent = 0;
private:
	void SendAndWaitForResponse(const std::string &aIP);

	bool myListenQueued = false;
	bool myWaiting = false;
	char myWriteBuffer[DEFAULT_BUFLEN];
	SOCKET myServerSocket = INVALID_SOCKET;


	//PROFILE
	int myServerBytesSent = 0;
	int myBytesGot = 0;
	float myPrintTimer = 0.0f;
	float myPing = 0.0f;
	std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();
};

