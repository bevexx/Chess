#pragma once
#include "Piece.h"
class Pawn : public Piece
{
public:
	Pawn(bool aIsWhite);
	virtual bool IsValidMove(Square *aSquare0, Square *aSquare1) const override;
	void OnMoved() override;
};

