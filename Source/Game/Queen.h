#pragma once
#include "Piece.h"
class Queen : public Piece
{
public:
	Queen(bool aIsWhite);
	virtual bool IsValidMove(Square *aSquare0, Square *aSquare1) const override;
private:

};

