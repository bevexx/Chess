#include "CNetMessageVector2f.h"
#include <string.h>

CNetMessageVector2f::CNetMessageVector2f() : myMessage()
{
    myClassTypeID = ClassTypeID::Vector2fMessage;
}

void CNetMessageVector2f::SetVector2f(const Vector2f &aFloat)
{
    memcpy(myMessage, &aFloat, sizeof(float)*2);
}

Vector2f CNetMessageVector2f::GetVector2f()
{
    Vector2f out;
    memcpy(&out, &myMessage, sizeof(float)*2);
    return out;
}
