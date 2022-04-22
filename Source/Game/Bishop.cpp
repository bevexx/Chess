#include "stdafx.h"
#include "Bishop.h"
#include "Square.h"
#include <tga2d/sprite/sprite.h>

Bishop::Bishop(bool aIsWhite)
{
	mySprite = new Tga2D::CSprite("sprites/bishop.png");
	if (!aIsWhite)
	{
		mySprite->SetColor({0.0f,0.5f,0.0f,1.0f});
	}
	Init(aIsWhite, 0.12f);
}

bool Bishop::IsValidMove(Square *aSquare0, Square *aSquare1) const
{
	return aSquare0->IsInSameDiagonalAs(aSquare1);
}