#include "stdafx.h"
#include "King.h"
#include "Square.h"
#include "Rook.h"
#include "GameWorld.h"
#include <tga2d/sprite/sprite.h>

King::King(bool aIsWhite)
{
	myRooks = std::make_pair(nullptr, nullptr);
	mySprite = new Tga2D::CSprite(aIsWhite ? "sprites/king.png" : "sprites/kingblack.png");
	Init(aIsWhite, 0.062f);
}

void King::OnMoved()
{
	if (std::abs(4 - mySquare->myPosition.x) >= 2 && !myHasMoved)
	{
		if (mySquare->myPosition.x < 4)
		{
			myRooks.first->mySquare->myPiece = nullptr;
			myRooks.first->mySquare = mySquare->mySquareList[mySquare->myPosition.x + 1][mySquare->myPosition.y];
			myRooks.first->mySquare->myPiece = this;
			myRooks.first->mySprite->SetPosition(myRooks.first->mySquare->mySprite->GetPosition());
		}
		else
		{
			myRooks.second->mySquare->myPiece = nullptr;
			myRooks.second->mySquare = mySquare->mySquareList[mySquare->myPosition.x - 1][mySquare->myPosition.y];
			myRooks.second->mySquare->myPiece = this;
			myRooks.second->mySprite->SetPosition(myRooks.second->mySquare->mySprite->GetPosition());
		}
	}
	myHasMoved = true;
}

bool King::IsValidMove(Square *aSquare0, Square *aSquare1) const
{
	//CASTLE
	if ((4 == aSquare1->myPosition.x + 2 || 4 == aSquare1->myPosition.x - 2) && (myIsWhite ? aSquare1->myPosition.y == 7 : aSquare1->myPosition.y == 0) && !myHasMoved && !myRooks.first->myHasMoved && !myRooks.second->myHasMoved)
	{
		for (auto &piece : myGameWorld->myBoard.myPieces)
		{
			if (piece->myIsWhite == myIsWhite || dynamic_cast<King*>(piece))
			{
				continue;
			}

			if (piece->IsValidMove(piece->mySquare, aSquare0->mySquareList[2][aSquare0->myPosition.y])) return false;
			if (piece->IsValidMove(piece->mySquare, aSquare0->mySquareList[3][aSquare0->myPosition.y])) return false;
			if (piece->IsValidMove(piece->mySquare, aSquare0->mySquareList[5][aSquare0->myPosition.y])) return false;
			if (piece->IsValidMove(piece->mySquare, aSquare0->mySquareList[6][aSquare0->myPosition.y])) return false;
		}
		return true;
	}

	return aSquare0->IsInSameStraightAs(aSquare1, 1) || aSquare0->IsInSameDiagonalAs(aSquare1, 1);
}