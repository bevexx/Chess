#pragma once
#include "Piece.h"
class Rook;
class King : public Piece
{
public:
	King(bool aIsWhite);
	virtual bool IsValidMove(Square *aSquare0, Square *aSquare1) const override;

	void OnMoved() override;

	std::pair<Rook*, Rook*> myRooks;
private:
};

