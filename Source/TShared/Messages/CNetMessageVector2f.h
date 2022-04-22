#pragma once
#include "CNetMessage.h"

struct Vector2f
{
	float x;
	float y;
};

class CNetMessageVector2f : public CNetMessage
{
public:
	CNetMessageVector2f();
	void SetVector2f(const Vector2f& aFloat);
	Vector2f GetVector2f();
private:
	char myMessage[8];
};