#include "stdafx.h"
#include "Horse.h"
#include "Square.h"
#include <tga2d/sprite/sprite.h>

Horse::Horse(bool aIsWhite)
{
	mySprite = new Tga2D::CSprite(aIsWhite ? "sprites/horse.png" : "sprites/horseblack.png");
	Init(aIsWhite, 0.061f);
}

bool Horse::IsValidMove(Square *aSquare0, Square *aSquare1) const
{
	return aSquare0->IsInHorseShapeFrom(aSquare1);
}