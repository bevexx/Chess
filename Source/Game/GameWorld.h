#pragma once
#include <tga2d\sprite\sprite.h>
#include "Pawn.h"
#include <map>
#include <unordered_map>
#include <thread>

#define SQUARE_SIZE 0.0975f

class King;
class Piece;
class Client;
class Square;
class ChessAI;

struct Board
{
	std::vector<Piece *> myPieces;
	std::vector<Piece *> myWhitePieces;
	std::vector<Piece *> myBlackPieces;
	std::vector<std::vector<Square *>> mySquares;
	King *myWhiteKing = nullptr;
	King *myBlackKing = nullptr;
	bool myIsWhitesTurn = true;
	bool myCheckmate = false;
};

class CGameWorld
{
public:
	CGameWorld(); 
	~CGameWorld();

	void Init(Client *aClient);
	void CreateBoard();
	void Update(float aTimeDelta);

	void FlipBoard();

	void SendMove(const VECTOR2I &aFrom, const VECTOR2I &aTo);
	void MakeMove(const VECTOR2I &aFrom, const VECTOR2I &aTo);

	void OnClick(const VECTOR2F& aPosition);
	void OnRelease(const VECTOR2F& aPosition);
	void RemovePiece(int aIndex);

	void SetLastMove(const VECTOR2I &aFrom, const VECTOR2I &aTo);
	bool IsInCheck(bool aIsWhite, Board *aBoard);
	bool IsInCheckAfter(Piece* aLastPiece, Square* aSquare, Board* aBoard);
	bool CheckIfCheckmateOnBoard(Board *aBoard);

	Board myBoard;

	int myPickedUpPiece = -1;
	std::pair<VECTOR2I, VECTOR2I> myLastMove;

	bool myClientIsWhite = true;


	Client *myClient = nullptr;
	Pawn *myEnPassentTarget = nullptr;

	bool myBoardIsFlipped = false;
	bool myPlayingLocally = true;

	bool myShouldThread = false;
	bool myPlayingWithAI = true;
	ChessAI *myChessAI = nullptr;
private:
};