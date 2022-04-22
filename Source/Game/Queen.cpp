#include "stdafx.h"
#include "Queen.h"
#include <tga2d/sprite/sprite.h>
#include "Square.h"

Queen::Queen(bool aIsWhite)
{
	mySprite = new Tga2D::CSprite(aIsWhite ? "sprites/queen.png" : "sprites/queenblack.png");
	Init(aIsWhite, 0.07f);
}

bool Queen::IsValidMove(Square *aSquare0, Square *aSquare1) const
{
	return aSquare0->IsInSameStraightAs(aSquare1) || aSquare0->IsInSameDiagonalAs(aSquare1);
}