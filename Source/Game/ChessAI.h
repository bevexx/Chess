#pragma once
#include "GameWorld.h"
#include <chrono>
#define int64 long long
#define AllWhite 12
#define AllBlack 13
#define All 14

enum class EPieceType : short { Knight=0, Bishop, Queen, Pawn, Rook, King};

struct Move
{
	std::pair<VECTOR2I, VECTOR2I> GetAsVector() const;
	float myValue = -1.0f;
	EPieceType myType = EPieceType::Queen;
	int64 myStart = 0;
	int64 myEnd = 0;
};

struct AIBoard
{
	AIBoard();
	AIBoard(const Board &aBoard);
	AIBoard(const AIBoard &aBoard);

	int64 GetKnightMoves(const int64 &piecePos) const;
	int64 GetBishopMoves(const int64 &piecePos, bool isWhite) const;
	int64 GetQueenMoves(const int64 &piecePos, bool isWhite) const;
	int64 GetPawnMoves(const int64 &piecePos, bool pieceIsWhite) const;
	int64 GetRookMoves(const int64 &piecePos, bool isWhite) const;
	int64 GetKingMoves(const int64 &piecePos) const;

	std::vector<Move> GetAllMoves(bool aIsWhite);
	std::vector<Move> GetAllOnBitBoardMoves(int64 aPosition, const EPieceType &aType);

	void MakeMove(const Move &move, bool aIsWhite);
	void UnMakeMove(const Move &move, bool aIsWhite);
	bool IsCheckmateOnBoard() const;
	float Evaluate() const;
	int CountAmountOfBits(int64 aValue) const;

	std::vector<int64> myLastState;
	int64 myLastWhitePieces = 0;
	int64 myLastBlackPieces = 0;
	int64 myLastAllPieces = 0;

	std::vector<int64*> myBitBoards;

	int64 myWhiteKnight = 0;
	int64 myWhiteBishop = 0;
	int64 myWhiteQueen = 0;
	int64 myWhitePawn = 0;
	int64 myWhiteRook = 0;
	int64 myWhiteKing = 0;

	int64 myBlackKnight = 0;
	int64 myBlackBishop = 0;
	int64 myBlackQueen = 0;
	int64 myBlackPawn = 0;
	int64 myBlackRook = 0;
	int64 myBlackKing = 0;

	int64 myWhitePieces = 0;
	int64 myBlackPieces = 0;
	int64 myAllPieces = 0;

	char myDirectionOffsets[8];
};

class ChessAI
{
	friend AIBoard;
	friend Move;

public:
	ChessAI();
	Move FindBestMove(const Board &aCurrentBoard);
	Move FindBestMove_Internal(AIBoard &aCurrentBoard, const bool &aMaxingWhite, int aDepth = 3, float aAlpha = -FLT_MAX, float aBeta = FLT_MAX);
	void StartTimer();

	int HasSeenPositionBefore(AIBoard &aCurrentBoard, bool aIsWhite);

	Move myBestMove;
	std::chrono::system_clock::time_point myStartTime;
	float myThinkTime = 5000.0f;

	CGameWorld *myGameWorld = nullptr;
	int myDepth = 6;
	int myDepthMin = 4;

private:
	static int64 myClearFile[8];
	static int64 myMaskRank[8];
	std::vector<std::pair<AIBoard, Move>> myPreviousPositionsBlack;
	std::vector<std::pair<AIBoard, Move>> myPreviousPositionsWhite;
	bool myTimeIsOut = false;
	std::thread myTimeThread;
};

