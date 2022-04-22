#pragma once
#include "CNetMessage.h"
#include <string>
class CNetMessageChatMessage : public CNetMessage
{
public:
	CNetMessageChatMessage();
	void SetMessage(const std::string &aMessage);
	std::string GetText();
private:
	char myMessage[256];
};