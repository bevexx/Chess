#include "stdafx.h"
#include "ChessAI.h"
#include "Bishop.h"
#include "Horse.h"
#include "Pawn.h"
#include "Queen.h"
#include "Rook.h"
#include "King.h"
#include "Square.h"



int64 ChessAI::myClearFile[8] ={ 0xFEFEFEFEFEFEFEFE,
								0xFDFDFDFDFDFDFDFD,
								0xFBFBFBFBFBFBFBFB,
								0xF7F7F7F7F7F7F7F7,
								0xEFEFEFEFEFEFEFEF,
								0xDFDFDFDFDFDFDFDF,
								0xBFBFBFBFBFBFBFBF,
								0x7F7F7F7F7F7F7F7F };

int64 ChessAI::myMaskRank[8] ={ 0xFF,
								0xFF00,
								0xFF0000,
								0xFF000000,
								0xFF00000000,
								0xFF0000000000,
								0xFF000000000000,
								0xFF00000000000000 };


AIBoard::AIBoard()
{
	myDirectionOffsets[0] = 1;
	myDirectionOffsets[1] = -1;
	myDirectionOffsets[2] = 8;
	myDirectionOffsets[3] = -8;
	myDirectionOffsets[4] = 7;
	myDirectionOffsets[5] = -7;
	myDirectionOffsets[6] = 9;
	myDirectionOffsets[7] = -9;

	myBitBoards.resize(12);
	myBitBoards[0] = &myWhiteKnight;
	myBitBoards[1] = &myWhiteBishop;
	myBitBoards[2] = &myWhiteQueen;
	myBitBoards[3] = &myWhitePawn;
	myBitBoards[4] = &myWhiteRook;
	myBitBoards[5] = &myWhiteKing;
	myBitBoards[6] = &myBlackKnight;
	myBitBoards[7] = &myBlackBishop;
	myBitBoards[8] = &myBlackQueen;
	myBitBoards[9] = &myBlackPawn;
	myBitBoards[10] = &myBlackRook;
	myBitBoards[11] = &myBlackKing;
}

AIBoard::AIBoard(const AIBoard &aBoard)
{
	myDirectionOffsets[0] = 1;
	myDirectionOffsets[1] = -1;
	myDirectionOffsets[2] = 8;
	myDirectionOffsets[3] = -8;
	myDirectionOffsets[4] = 7;
	myDirectionOffsets[5] = -7;
	myDirectionOffsets[6] = 9;
	myDirectionOffsets[7] = -9;

	myBitBoards.resize(12);
	myBitBoards[0] = &myWhiteKnight;
	myBitBoards[1] = &myWhiteBishop;
	myBitBoards[2] = &myWhiteQueen;
	myBitBoards[3] = &myWhitePawn;
	myBitBoards[4] = &myWhiteRook;
	myBitBoards[5] = &myWhiteKing;
	myBitBoards[6] = &myBlackKnight;
	myBitBoards[7] = &myBlackBishop;
	myBitBoards[8] = &myBlackQueen;
	myBitBoards[9] = &myBlackPawn;
	myBitBoards[10] = &myBlackRook;
	myBitBoards[11] = &myBlackKing;

	myWhitePieces = aBoard.myWhitePieces;
	myBlackPieces = aBoard.myBlackPieces;
	myAllPieces = aBoard.myAllPieces;

	for (int i = 0; i < myBitBoards.size(); i++)
	{
		*myBitBoards[i] = *aBoard.myBitBoards[i];
	}
}

AIBoard::AIBoard(const Board &aBoard)
{
	myDirectionOffsets[0] = 1;
	myDirectionOffsets[1] = -1;
	myDirectionOffsets[2] = 8;
	myDirectionOffsets[3] = -8;
	myDirectionOffsets[4] = 7;
	myDirectionOffsets[5] = -7;
	myDirectionOffsets[6] = 9;
	myDirectionOffsets[7] = -9;

	myBitBoards.resize(12);

	myBitBoards[0] = &myWhiteKnight;
	myBitBoards[1] = &myWhiteBishop;
	myBitBoards[2] = &myWhiteQueen;
	myBitBoards[3] = &myWhitePawn;
	myBitBoards[4] = &myWhiteRook;
	myBitBoards[5] = &myWhiteKing;

	myBitBoards[6] = &myBlackKnight;
	myBitBoards[7] = &myBlackBishop;
	myBitBoards[8] = &myBlackQueen;
	myBitBoards[9] = &myBlackPawn;
	myBitBoards[10] = &myBlackRook;
	myBitBoards[11] = &myBlackKing;

	for (Piece* piece : aBoard.myPieces)
	{
		if (!piece->mySquare)
		{
			continue;
		}

		int64 piecePos = ~ChessAI::myClearFile[piece->mySquare->myPosition.x];
		piecePos &= ChessAI::myMaskRank[piece->mySquare->myPosition.y];

		if (piece->myIsWhite)
		{
			myWhitePieces |= piecePos;
		}
		else
		{
			myBlackPieces |= piecePos;
		}
		myAllPieces |= piecePos;
		
		if (dynamic_cast<Horse *>(piece))
		{
			if (piece->myIsWhite)
			{
				myWhiteKnight |= piecePos;
			}
			else
			{
				myBlackKnight |= piecePos;
			}
		}
		else if (dynamic_cast<Bishop *>(piece))
		{
			if (piece->myIsWhite)
			{
				myWhiteBishop |= piecePos;
			}
			else
			{
				myBlackBishop |= piecePos;
			}
		}
		else if (dynamic_cast<Queen *>(piece))
		{
			if (piece->myIsWhite)
			{
				myWhiteQueen |= piecePos;
			}
			else
			{
				myBlackQueen |= piecePos;
			}
		}
		else if (dynamic_cast<Rook *>(piece))
		{
			if (piece->myIsWhite)
			{
				myWhiteRook |= piecePos;
			}
			else
			{
				myBlackRook |= piecePos;
			}
		}
		else if (dynamic_cast<Pawn *>(piece))
		{
			if (piece->myIsWhite)
			{
				myWhitePawn |= piecePos;
			}
			else
			{
				myBlackPawn |= piecePos;
			}
		}
		else if (dynamic_cast<King *>(piece))
		{
			if (piece->myIsWhite)
			{
				myWhiteKing |= piecePos;
			}
			else
			{
				myBlackKing |= piecePos;
			}
		}
	}
}

int64 AIBoard::GetKnightMoves(const int64 &piecePos) const
{
	int64 spot_1_clip = ChessAI::myClearFile[0] & ChessAI::myClearFile[1];
	int64 spot_2_clip = ChessAI::myClearFile[0];
	int64 spot_3_clip = ChessAI::myClearFile[7];
	int64 spot_4_clip = ChessAI::myClearFile[7] & ChessAI::myClearFile[6];

	int64 spot_5_clip = ChessAI::myClearFile[7] & ChessAI::myClearFile[6];
	int64 spot_6_clip = ChessAI::myClearFile[7];
	int64 spot_7_clip = ChessAI::myClearFile[0];
	int64 spot_8_clip = ChessAI::myClearFile[0] & ChessAI::myClearFile[1];

	int64 spot_1 = (piecePos & spot_1_clip) << 6;
	int64 spot_2 = (piecePos & spot_2_clip) << 15;
	int64 spot_3 = (piecePos & spot_3_clip) << 17;
	int64 spot_4 = (piecePos & spot_4_clip) << 10;

	int64 spot_5 = (piecePos & spot_5_clip) >> 6;
	int64 spot_6 = (piecePos & spot_6_clip) >> 15;
	int64 spot_7 = (piecePos & spot_7_clip) >> 17;
	int64 spot_8 = (piecePos & spot_8_clip) >> 10;

	return spot_1 | spot_2 | spot_3 | spot_4 | spot_5 | spot_6 | spot_7 | spot_8;
}

int64 AIBoard::GetBishopMoves(const int64 &piecePos, bool isWhite) const
{
	int64 moves = 0;
	int64 clipFileH = piecePos & ChessAI::myClearFile[7];
	int64 clipFileA = piecePos & ChessAI::myClearFile[0];
	int64 friendlyPieces = isWhite ? myWhitePieces : myBlackPieces;

	for (char direction = 4; direction < 8; direction++)
	{
		for (char index = 1; index < 8; index++)
		{
			int64 spot;
			if (myDirectionOffsets[direction] > 0)
			{
				spot = clipFileH << myDirectionOffsets[direction] * index;
				spot &= friendlyPieces;
			}
			else
			{
				spot = clipFileA >> -myDirectionOffsets[direction] * index;
				spot &= friendlyPieces;
			}

			moves |= spot;

			if (spot == 0)
			{
				break;
			}
		}
	}
	return moves;
}

int64 AIBoard::GetQueenMoves(const int64 &piecePos, bool isWhite) const
{
	int64 moves = 0;
	int64 clipFileH = piecePos & ChessAI::myClearFile[7];
	int64 clipFileA = piecePos & ChessAI::myClearFile[0];
	int64 friendlyPieces = isWhite ? myWhitePieces : myBlackPieces;

	for (char direction = 0; direction < 8; direction++)
	{
		for (char index = 1; index < 8; index++)
		{
			int64 spot;
			if (myDirectionOffsets[direction] > 0)
			{
				spot = clipFileH << myDirectionOffsets[direction] * index;
				spot &= friendlyPieces;
			}
			else
			{
				spot = clipFileA >> -myDirectionOffsets[direction] * index;
				spot &= friendlyPieces;
			}

			moves |= spot;

			if (spot == 0)
			{
				break;
			}
		}
	}
	return moves;
}

int64 AIBoard::GetRookMoves(const int64 &piecePos, bool isWhite) const
{
	int64 moves = 0;
	int64 clipFileH = piecePos & ChessAI::myClearFile[7];
	int64 clipFileA = piecePos & ChessAI::myClearFile[0];

	for (char direction = 0; direction < 4; direction++)
	{
		for (char index = 1; index < 8; index++)
		{
			int64 spot;
			if (myDirectionOffsets[direction] > 0)
			{
				spot = clipFileH << myDirectionOffsets[direction] * index;
				spot &= (isWhite ? ~myWhitePieces : ~myBlackPieces);
			}
			else
			{
				spot = clipFileA >> -myDirectionOffsets[direction] * index;
				spot &= (isWhite ? ~myWhitePieces : ~myBlackPieces);
			}

			moves |= spot;

			if (spot == 0)
			{
				break;
			}
		}
	}
	return moves;
}

int64 AIBoard::GetPawnMoves(const int64 &piecePos, bool pieceIsWhite) const
{
	int64 white_pawn_one_step = (piecePos << 8) & ~myAllPieces;

	/* for all moves that came from rank 2 (home row) and passed the above
		filter, thereby being on rank 3, check and see if I can move forward
		one more */
	int64 white_pawn_two_steps = ((white_pawn_one_step & ChessAI::myMaskRank[2]) << 8) & ~myAllPieces;

	/* the union of the movements dictate the possible moves forward
		available */
	int64 white_pawn_valid_moves = white_pawn_one_step | white_pawn_two_steps;

	/* next we calculate the pawn attacks */

	/* check the left side of the pawn, minding the underflow File A */
	int64 white_pawn_left_attack = (piecePos & ChessAI::myClearFile[0]) << 7;

	/* then check the right side of the pawn, minding the overflow File H */
	int64 white_pawn_right_attack = (piecePos & ChessAI::myClearFile[7]) << 9;

	/* the union of the left and right attacks together make up all the
		possible attacks */
	int64 white_pawn_attacks = white_pawn_left_attack | white_pawn_right_attack;

	/* Calculate where I can _actually_ attack something */
	int64 white_pawn_valid_attacks = white_pawn_attacks & (pieceIsWhite ? myBlackPieces : myWhitePieces);

	/* then we combine the two situations in which a white pawn can legally
		attack/move. */
	return white_pawn_valid_moves | white_pawn_valid_attacks;
}

int64 AIBoard::GetKingMoves(const int64 &piecePos) const
{
	int64 clipFileH = piecePos & ChessAI::myClearFile[7];
	int64 clipFileA = piecePos & ChessAI::myClearFile[0];

	int64 spot_1 = clipFileH << 7;
	int64 spot_2 = piecePos << 8;
	int64 spot_3 = clipFileH << 9;
	int64 spot_4 = clipFileH << 1;

	int64 spot_5 = clipFileA >> 7;
	int64 spot_6 = piecePos >> 8;
	int64 spot_7 = clipFileA >> 9;
	int64 spot_8 = clipFileA >> 1;

	return spot_1 | spot_2 | spot_3 | spot_4 | spot_5 | spot_6 | spot_7 | spot_8;
}

std::vector<Move> AIBoard::GetAllMoves(bool aIsWhite)
{
	std::vector<Move> allMoves;
	allMoves.reserve(100);
	for (int i = 0 + (aIsWhite ? 0 : 6); i <= static_cast<short>(EPieceType::King) + (aIsWhite ? 0 : 6); i++)
	{
		std::vector<Move> moves = GetAllOnBitBoardMoves(*myBitBoards[i], static_cast<EPieceType>(i - (aIsWhite ? 0 : 6)));
		allMoves.insert(allMoves.end(), moves.begin(), moves.end());
	}
	return allMoves;
}

std::vector<Move> AIBoard::GetAllOnBitBoardMoves(int64 aPieces, const EPieceType &aType)
{
	bool isWhite = aPieces == myWhitePieces;
	short index = static_cast<short>(aType);
	std::vector<Move> out;

	int64 piecePos = 1;
	while (piecePos)
	{
		piecePos = aPieces & -aPieces; // Get first piece bit
		aPieces &= ~piecePos; // Remove bit from unexplored
		if (aPieces == 0 && piecePos == 0)
		{
			continue;
		}
		
		int64 moves = 0;
		if (index == 0) // Knight
		{
			moves = GetKnightMoves(piecePos);
		}
		else if (index == 1) // Bishop
		{
			moves = GetBishopMoves(piecePos, isWhite);
		}
		else if (index == 2) // Queen
		{
			moves = GetQueenMoves(piecePos, isWhite);
		}
		else if (index == 3) // Pawn
		{
			moves = GetPawnMoves(piecePos, isWhite);
		}
		else if (index == 4) // Rook
		{
			moves = GetRookMoves(piecePos, isWhite);
		}
		else // King
		{
			moves = GetKingMoves(piecePos);
		}

		if (isWhite)
		{
			moves &= ~myWhitePieces;
		}
		else
		{
			moves &= ~myBlackPieces;
		}

		int64 second = 1;
		while (second)
		{
			second = moves & -moves; // Get first square bit
			moves &= ~second; // Remove bit from unexplored
			if (moves == 0 && second == 0)
			{
				continue;
			}

			out.push_back(Move{ 0.0f, aType, piecePos, second });
		}
	}

	return out;
}

void AIBoard::MakeMove(const Move &move, bool aIsWhite)
{
	myBlackPieces = myBlackPieces;
	myLastWhitePieces = myWhitePieces;
	myLastAllPieces = myAllPieces;

	int index = static_cast<int>(move.myType) + (aIsWhite ? 0 : 6);
	for (int i = (aIsWhite ? 6 : 0); i < myBitBoards.size() - (aIsWhite ? 0 : 6); i++)
	{
		myLastState.push_back(*myBitBoards[i]);

		if (i == index)
		{
			continue;
		}

		if (*myBitBoards.at(i) & move.myEnd)
		{
			*myBitBoards.at(i) &= ~move.myEnd;
			break;
		}
	}

	*myBitBoards.at(index) &= ~move.myStart;
	*myBitBoards.at(index) |= move.myEnd;

	if (index >= 6)
	{
		myBlackPieces &= ~move.myStart;
		myBlackPieces |= move.myEnd;
	}
	else
	{
		myWhitePieces &= ~move.myStart;
		myWhitePieces |= move.myEnd;
	}

	myAllPieces &= ~move.myStart;
	myAllPieces |= move.myEnd;
}

void AIBoard::UnMakeMove(const Move &move, bool aIsWhite)
{
	int index = static_cast<int>(move.myType) + (aIsWhite ? 0 : 6);
	for (int i = (aIsWhite ? 6 : 0); i < myBitBoards.size() - (aIsWhite ? 0 : 6); i++)
	{
		*myBitBoards.at(i) = myLastState.at(i);
	}

	*myBitBoards.at(index) &= ~move.myEnd;
	*myBitBoards.at(index) |= move.myStart;

	myBlackPieces = myLastBlackPieces;
	myWhitePieces = myLastWhitePieces;
	myAllPieces = myLastAllPieces;
}

bool AIBoard::IsCheckmateOnBoard() const
{
	return false;
}

int AIBoard::CountAmountOfBits(int64 aValue) const
{
	unsigned int count = 0;
	while (aValue)
	{
		aValue &= (aValue - 1);
		count++;
	}
	return count;
}

float AIBoard::Evaluate() const
{
	float count = 0.0f;
	float score = 0.0f;
	for (int i = 0; i < myBitBoards.size(); i++)
	{
		switch (static_cast<EPieceType>(i - (i < 6 ? 0 : 6)))
		{
		case EPieceType::Knight:
			score = 2.8f;
			break;
		case EPieceType::Bishop:
			score = 3.1f;
			break;
		case EPieceType::Queen:
			score = 9.0f;
			break;
		case EPieceType::Pawn:
			score = 1.0f;
			break;
		case EPieceType::Rook:
			score = 5.0f;
			break;
		case EPieceType::King:
			score = 1000.0f;
			break;
		default:
			break;
		}
		float amount = (float)CountAmountOfBits(*myBitBoards[i]);
		float side = (i > 5 ? -1.0f : 1.0f);
		float centerControl = 0.3f * (float)CountAmountOfBits((*myBitBoards[i] & (~ChessAI::myClearFile[3] | ~ChessAI::myClearFile[4]) & (ChessAI::myMaskRank[3] | ChessAI::myMaskRank[4])));

		count += (amount * score /*+ centerControl */) * side;
	}
	return count;
}

std::pair<VECTOR2I, VECTOR2I> Move::GetAsVector() const
{
	VECTOR2I start;
	VECTOR2I end;

	for (int i = 0; i < 8; i++)
	{
		if ((myStart & ~ChessAI::myClearFile[i]) != 0)
		{
			start.x = i;
			break;
		}
	}
	for (int i = 0; i < 8; i++)
	{
		if ((myStart & ChessAI::myMaskRank[i]) != 0)
		{
			start.y = i;
			break;
		}
	}
	for (int i = 0; i < 8; i++)
	{
		if ((myEnd & ~ChessAI::myClearFile[i]) != 0)
		{
			end.x = i;
			break;
		}
	}
	for (int i = 0; i < 8; i++)
	{
		if ((myEnd & ChessAI::myMaskRank[i]) != 0)
		{
			end.y = i;
			break;
		}
	}

	return std::make_pair(start, end);
}

ChessAI::ChessAI()
{
	
}

void ChessAI::StartTimer()
{
	myStartTime = std::chrono::system_clock::now();
	while (!myTimeIsOut)
	{
		float timeSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - myStartTime).count();
		if (timeSinceStart > myThinkTime && myBestMove.myValue != -1)
		{
			myTimeIsOut = true;
			auto move = myBestMove.GetAsVector();
			myGameWorld->MakeMove(move.first, move.second);
			//std::cout << "depth: " << myDepth << "\n";
		}
	}
}


Move ChessAI::FindBestMove(const Board &aCurrentBoard)
{
	myPreviousPositionsWhite.clear();
	myPreviousPositionsBlack.clear();
	myPreviousPositionsWhite.reserve(1000000/2);
	myPreviousPositionsBlack.reserve(1000000/2);

	if (!myGameWorld->myShouldThread)
	{
		return FindBestMove_Internal(AIBoard(aCurrentBoard), aCurrentBoard.myIsWhitesTurn, myDepth);
	}

	myTimeIsOut = false;
	myDepth = myDepthMin;

	//std::thread timerThread([=]() { this->StartTimer(); });
	//timerThread.detach();

	myStartTime = std::chrono::system_clock::now();
	while (!myTimeIsOut)
	{
		float timeSinceStart = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - myStartTime).count();
		if (timeSinceStart > myThinkTime && myBestMove.myValue != -1)
		{
			myTimeIsOut = true;
			auto move = myBestMove.GetAsVector();
			myGameWorld->MakeMove(move.first, move.second);
			//std::cout << "depth: " << myDepth << "\n";
			continue;
		}

		myBestMove = FindBestMove_Internal(AIBoard(aCurrentBoard), aCurrentBoard.myIsWhitesTurn, myDepth);
		myDepth++;
	}

	return myBestMove;
}

Move ChessAI::FindBestMove_Internal(AIBoard &aCurrentBoard, const bool &aMaxingWhite, int aDepth, float aAlpha, float aBeta)
{
	if (myTimeIsOut || aDepth <= 0 || aCurrentBoard.IsCheckmateOnBoard())
	{
		return Move{ aCurrentBoard.Evaluate() };
	}

	Move bestMove{ aMaxingWhite ? -MAXINT32 : MAXINT32 };
	std::vector allMoves = aCurrentBoard.GetAllMoves(aMaxingWhite);

	if (aMaxingWhite)
	{
		for (Move move : allMoves)
		{
			aCurrentBoard.MakeMove(move, true);
			move.myValue = FindBestMove_Internal(aCurrentBoard, false, aDepth - 1, aAlpha, aBeta).myValue;
			aCurrentBoard.UnMakeMove(move, true);

			if (move.myValue > bestMove.myValue)
			{
				bestMove = move;
			}

			if (move.myValue >= aBeta)
			{
				return bestMove;
			}

			aAlpha = max(aAlpha, bestMove.myValue);
		}
	}
	else
	{
		for (Move move : allMoves)
		{
			aCurrentBoard.MakeMove(move, false);
			move.myValue = FindBestMove_Internal(aCurrentBoard, true, aDepth - 1, aAlpha, aBeta).myValue;
			aCurrentBoard.UnMakeMove(move, false);

			if (move.myValue < bestMove.myValue)
			{
				bestMove = move;
			}

			if (move.myValue <= aAlpha)
			{
				return bestMove;
			}

			aBeta = min(aBeta, bestMove.myValue);
		}
	}

	//if (aMaxingWhite)
	//{
	//	myPreviousPositionsWhite.push_back(std::make_pair(aCurrentBoard, bestMove));
	//}
	//else
	//{
	//	myPreviousPositionsBlack.push_back(std::make_pair(aCurrentBoard, bestMove));
	//}

	return bestMove;
}

int ChessAI::HasSeenPositionBefore(AIBoard &aCurrentBoard, bool aIsWhite)
{
	auto list = aIsWhite ? myPreviousPositionsWhite : myPreviousPositionsBlack;
	for (int i = 0; i < list.size(); i++)
	{
		bool notIt = false;
		for (int j = 0; j < list[i].first.myBitBoards.size(); j++)
		{
			if (*list[i].first.myBitBoards[j] != *aCurrentBoard.myBitBoards[j])
			{
				notIt = true;
				break;
			}
		}
		if (!notIt)
		{
			return i;
		}
	}
	return -1;
}
