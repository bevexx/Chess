#include "CNetMessageChatMessage.h"

CNetMessageChatMessage::CNetMessageChatMessage()
	: myMessage()
{
	myClassTypeID = ClassTypeID::StringMessage;
}

void CNetMessageChatMessage::SetMessage(const std::string &aMessage)
{
	memcpy(myMessage, &aMessage[0], sizeof(char) * aMessage.size());
	myMessage[aMessage.size()] = '\0';
}

std::string CNetMessageChatMessage::GetText()
{
	return std::string(myMessage);
}

