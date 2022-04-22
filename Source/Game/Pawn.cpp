#include "stdafx.h"
#include "Pawn.h"
#include "Square.h"
#include "GameWorld.h"
#include <tga2d/sprite/sprite.h>

Pawn::Pawn(bool aIsWhite)
{
	mySprite = new Tga2D::CSprite(aIsWhite ? "sprites/pawn.png" : "sprites/pawnblack.png");
	Init(aIsWhite, 0.065f);
}

void Pawn::OnMoved()
{
	if (!myHasMoved)
	{
		myGameWorld->myEnPassentTarget = this;
	}
	Piece::OnMoved();
}

bool Pawn::IsValidMove(Square *aSquare0, Square *aSquare1) const
{
	if (!myIsWhite ? aSquare0->myPosition.y > aSquare1->myPosition.y : aSquare0->myPosition.y < aSquare1->myPosition.y || aSquare0->myPosition.y == aSquare1->myPosition.y)
	{
		return false;
	}

	// En passant
	Piece *piece = aSquare0->mySquareList[aSquare1->myPosition.x][aSquare1->myPosition.y + (myIsWhite ? 1 : -1)]->myPiece;
	if (piece)
	{
		if (Pawn* pawn = dynamic_cast<Pawn *>(piece))
		{
			if (pawn == myGameWorld->myEnPassentTarget && myIsWhite != pawn->myIsWhite)
			{
				myGameWorld->RemovePiece(pawn->myPieceIndex);
				return true;
			}
		}
	}

	if (aSquare0->IsInSameStraightAs(aSquare1, myHasMoved ? 1 : 2))
	{
		return !aSquare1->myPiece;
	}

	return aSquare1->myPiece ? aSquare0->IsInSameDiagonalAs(aSquare1, 1) : false;
}