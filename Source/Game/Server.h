#pragma once
#include <WinSock2.h>
#include "SharedDefines.h"
#include <unordered_map>
#include "Messages/CNetMessageVector2f.h"

class CGameWorld;

class Server
{
public:
	void Init();
	void StartListenThread();

private:
	void HandleStringMessage(const std::string &aData, SOCKADDR_IN &senderAddr);
	void StartGame();
	void Listen();

	SOCKET myClientSocket = INVALID_SOCKET;

	struct User
	{
		SOCKADDR_IN myAddr;
		bool myIsWhite;
	};

	std::vector<User> myUsers;
};

