#include "CNetMessageVector2i.h"
#include <string.h>

CNetMessageVector2i::CNetMessageVector2i() : myMessage()
{
    myClassTypeID = ClassTypeID::Vector2iMessage;
}

void CNetMessageVector2i::SetVector2i(const Vector2i &anInt0)
{
    memcpy(myMessage, &anInt0, sizeof(int) * 2);
}

void CNetMessageVector2i::Set2Vector2i(const Vector2i &anInt0, const Vector2i &anInt1)
{
    std::pair<Vector2i, Vector2i> pair = std::make_pair(anInt0, anInt1);
    memcpy(myMessage, &pair, sizeof(std::pair<Vector2i, Vector2i>));
}

Vector2i CNetMessageVector2i::GetVector2i()
{
    Vector2i out;
    memcpy(&out, &myMessage, sizeof(int)*2);
    return out;
}

std::pair<Vector2i, Vector2i> CNetMessageVector2i::Get2Vector2i()
{
    std::pair<Vector2i, Vector2i> out;
    memcpy(&out, &myMessage, sizeof(std::pair<Vector2i, Vector2i>));
    return out;
}
