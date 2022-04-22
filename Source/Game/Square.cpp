#include "stdafx.h"
#include "Square.h"
#include "Rook.h"
#include "Bishop.h"
#include "Horse.h"
#include "King.h"
#include "Pawn.h"
#include "Queen.h"
#include <tga2d/sprite/sprite.h>
#include "GameWorld.h"

Square::Square(const VECTOR2I &pos, std::vector<std::vector<Square *>> &aSquareList) : mySquareList(aSquareList)
{
	myPosition = pos;

	mySprite = new Tga2D::CSprite("sprites/square.png");
	mySprite->SetPivot({ 0.5f,0.5f });
	float size = SQUARE_SIZE;
	mySprite->SetSizeRelativeToScreen({ size,size });
	VECTOR2F worldPos = { 0.175f + pos.x * size, 0.125f + pos.y * size };
	mySprite->SetPosition(worldPos);

	if ((pos.x + pos.y) % 2 == 1)
	{
		mySprite->SetColor(GREEN_COLOR);
	}
	else
	{
		mySprite->SetColor(OFF_WHITE_COLOR);
	}

	if (pos.y >= 2 && pos.y <= 5)
	{
		return;
	}

	if (pos.y == 0 || pos.y == 7)
	{
		if (pos.x == 0 || pos.x == 7)
		{
			myPiece = new Rook(pos.y > 3);
		}
		else if (pos.x == 1 || pos.x == 6)
		{
			myPiece = new Horse(pos.y > 3);
		}
		else if (pos.x == 2 || pos.x == 5)
		{
			myPiece = new Bishop(pos.y > 3);
		}
		else if (pos.x == 4)
		{
			myPiece = new King(pos.y > 3);
		}
		else
		{
			myPiece = new Queen(pos.y > 3);
		}

		myPiece->mySprite->SetPosition(worldPos);
		myPiece->mySquare = this;
	}
	else if (pos.y == 1 || pos.y == 6)
	{
		myPiece = new Pawn(pos.y > 3);
		myPiece->mySprite->SetPosition(worldPos);
		myPiece->mySquare = this;
	}
}

bool Square::IsInSameStraightAs(Square *aSquare, int aRange)
{
	bool swapped = false;
	int changed0, changed1, other;
	if (myPosition.x == aSquare->myPosition.x)
	{
		if (std::abs(myPosition.y - aSquare->myPosition.y) > aRange)
		{
			return false;
		}

		changed0 = myPosition.y;
		changed1 = aSquare->myPosition.y;
		other = myPosition.x;
	}
	else if (myPosition.y == aSquare->myPosition.y)
	{
		if (std::abs(myPosition.x - aSquare->myPosition.x) > aRange)
		{
			return false;
		}

		changed0 = myPosition.x;
		changed1 = aSquare->myPosition.x;
		other = myPosition.y;
		swapped = true;
	}
	else
	{
		return false;
	}

	for (auto i = (changed0 < changed1 ? changed0 : changed1) + 1; i < (changed0 > changed1 ? changed0 : changed1); i++)
	{
		if (mySquareList[!swapped ? other : i][swapped ? other : i]->myPiece)
		{
			return false;
		}
	}

	return true;
}
bool Square::IsInSameDiagonalAs(Square *aSquare, int aRange)
{
	if (myPosition.x + aSquare->myPosition.y != myPosition.y + aSquare->myPosition.x && aSquare->myPosition.x + aSquare->myPosition.y != myPosition.x + myPosition.y)
	{
		return false;
	}
	else if (std::abs(myPosition.x - aSquare->myPosition.x) > aRange || std::abs(myPosition.y - aSquare->myPosition.y) > aRange)
	{
		return false;
	}

	int i = myPosition.x + (myPosition.x < aSquare->myPosition.x ? 1 : -1), j = myPosition.y + (myPosition.y < aSquare->myPosition.y ? 1 : -1);
	while (i != aSquare->myPosition.x && j != aSquare->myPosition.y)
	{
		if (mySquareList[i][j]->myPiece)
		{
			return false;
		}

		i += myPosition.x < aSquare->myPosition.x ? 1 : -1;
		j += myPosition.y < aSquare->myPosition.y ? 1 : -1;
	}

	return true;
}
bool Square::IsInHorseShapeFrom(Square *aSquare, int aRange)
{
	return (((myPosition.x == aSquare->myPosition.x - 2 || myPosition.x == aSquare->myPosition.x + 2) && (myPosition.y == aSquare->myPosition.y - 1 || myPosition.y == aSquare->myPosition.y + 1))
		|| ((myPosition.y == aSquare->myPosition.y - 2 || myPosition.y == aSquare->myPosition.y + 2) && (myPosition.x == aSquare->myPosition.x - 1 || myPosition.x == aSquare->myPosition.x + 1)));
}