#pragma once

#define GREEN_COLOR { 119.0f / 255.0f, 149.0f / 255.0f, 86.0f / 255.0f, 1.0f}
#define OFF_WHITE_COLOR { 235.0f/255.0f, 235.0f/255.0f, 208.0f/255.0f, 1.0f }

class Piece;
class Square
{
public:
	Square(const VECTOR2I &pos, std::vector<std::vector<Square*>>& aSquareList);

	bool IsInSameStraightAs(Square *aSquare, int aRange = 10);
	bool IsInSameDiagonalAs(Square *aSquare, int aRange = 10);
	bool IsInHorseShapeFrom(Square *aSquare, int aRange = 10);

	VECTOR2I myPosition;
	Piece* myPiece = nullptr;
	Tga2D::CSprite *mySprite = nullptr;

	std::vector<std::vector<Square*>>& mySquareList;
private:
};

