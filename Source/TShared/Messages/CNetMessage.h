#pragma once
#include "ClassTypeID.h"
#include <chrono>

class CNetMessage
{
public:
	ClassTypeID myClassTypeID = ClassTypeID::NoMessage;
	int myID = -1;
	bool myGameHasStarted = false;
};

