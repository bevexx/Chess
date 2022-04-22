#pragma once
#include "CNetMessage.h"

struct Vector2i
{
	int x;
	int y;
};

class CNetMessageVector2i : public CNetMessage
{
public:
	CNetMessageVector2i();
	void SetVector2i(const Vector2i &anInt);
	void Set2Vector2i(const Vector2i &anInt0, const Vector2i &anInt1);
	Vector2i GetVector2i();
	std::pair<Vector2i, Vector2i> Get2Vector2i();
private:
	char myMessage[20];
};