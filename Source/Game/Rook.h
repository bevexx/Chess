#pragma once
#include "Piece.h"
class Rook : public Piece
{
public:
	Rook(bool aIsWhite);
	virtual bool IsValidMove(Square *aSquare0, Square *aSquare1) const override;
private:

};

