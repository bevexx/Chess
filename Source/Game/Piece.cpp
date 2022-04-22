#include "stdafx.h"
#include "Piece.h"
#include <tga2d/sprite/sprite.h>
#include "GameWorld.h"
#include "Square.h"

void Piece::Init(bool aIsWhite, float aSize)
{
	mySprite->SetPivot({ 0.5f,0.5f });
	mySprite->SetSizeRelativeToImage({ aSize, aSize });
	myIsWhite = aIsWhite;
}

float Piece::OnClick(VECTOR2F aPos)
{
	if (!mySprite)
	{
		return -1.0f;
	}

	VECTOR2F diff = mySprite->GetPosition() - aPos;
	float distance = sqrt((diff.x * diff.x) + (diff.y * diff.y));
	return distance <= myPickUpRange*myPickUpRange ? distance : -1.0f;
}

std::vector<VECTOR2I> Piece::GetAllPossibleMoves() const
{
	return std::vector<VECTOR2I>();
}

bool Piece::CanMakeMove(Square *aSquare) const
{
	return (myGameWorld->myClientIsWhite == myGameWorld->myBoard.myIsWhitesTurn || myGameWorld->myPlayingLocally) && myGameWorld->myBoard.myIsWhitesTurn == myIsWhite && (!aSquare->myPiece || aSquare->myPiece->myIsWhite == !myIsWhite) && IsValidMove(mySquare, aSquare);
}

bool Piece::OnRelease(Square *aSquare)
{
	if (CanMakeMove(aSquare) && !myGameWorld->IsInCheckAfter(this, aSquare, &myGameWorld->myBoard))
	{
		if (myGameWorld->myEnPassentTarget != this)
		{
			myGameWorld->myEnPassentTarget = nullptr;
		}

		if (aSquare->myPiece)
		{
			myGameWorld->RemovePiece(aSquare->myPiece->myPieceIndex);
		}

		myGameWorld->SetLastMove(mySquare->myPosition, aSquare->myPosition);

		mySquare->myPiece = nullptr;
		mySquare = aSquare;
		aSquare->myPiece = this;
		mySprite->SetPosition(aSquare->mySprite->GetPosition());
		OnMoved();

		return true;
	}
	else
	{
		mySprite->SetPosition(mySquare->mySprite->GetPosition());
	}
	return false;
}

void Piece::Render()
{
	if (mySprite)
	{
		mySprite->Render();
	}
}