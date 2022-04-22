#pragma once
#include "Piece.h"
class Bishop : public Piece
{
public:
	Bishop(bool aIsWhite);
	virtual bool IsValidMove(Square *aSquare0, Square *aSquare1) const override;
private:

};

