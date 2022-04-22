#include "stdafx.h"
#include "Rook.h"
#include <tga2d/sprite/sprite.h>
#include "Square.h"

Rook::Rook(bool aIsWhite)
{
	mySprite = new Tga2D::CSprite(aIsWhite ? "sprites/rook.png" : "sprites/rookblack.png");
	Init(aIsWhite, 1.0f);
}

bool Rook::IsValidMove(Square *aSquare0, Square *aSquare1) const
{
	return aSquare0->IsInSameStraightAs(aSquare1);
}