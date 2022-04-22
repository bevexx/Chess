#pragma once
#include "Piece.h"
class Horse : public Piece
{
public:
	Horse(bool aIsWhite);
	virtual bool IsValidMove(Square *aSquare0, Square *aSquare1) const override;
private:

};

