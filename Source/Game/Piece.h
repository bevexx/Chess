#pragma once
class CGameWorld;
class Square;
class Piece
{
public:
	void Init(bool aIsWhite, float aSize = 1.0f);
	virtual bool IsValidMove(Square *aSquare0, Square *aSquare1) const { return false; }

	float OnClick(VECTOR2F);
	bool OnRelease(Square* aSquare);
	void Render();

	virtual std::vector<VECTOR2I> GetAllPossibleMoves() const;
	bool CanMakeMove(Square *aSquare) const;
	virtual void OnMoved() { myHasMoved = true; }

	Tga2D::CSprite *mySprite = nullptr;
	CGameWorld *myGameWorld = nullptr;
	Square *mySquare = nullptr;
	int myPieceIndex = -1;
	bool myIsWhite = true;
	float myPickUpRange = 0.22f;
	bool myHasMoved = false;

private:
	bool myIsPickedUp = false;
};

