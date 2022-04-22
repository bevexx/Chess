#include "stdafx.h"
#include "GameWorld.h"
#include "Client.h"
#include "Server.h"
#include "Square.h"
#include "King.h"
#include "Rook.h"
#include "Messages/CNetMessageVector2i.h"
#include <iostream>
#include "ChessAI.h"

#include <tga2d/drawers/debug_drawer.h>
#include <tga2d/error/error_manager.h>

CGameWorld::CGameWorld()
{
	myLastMove = std::make_pair(VECTOR2F{ -1, -1 }, VECTOR2F{ -1, -1 });
}

CGameWorld::~CGameWorld() 
{
	delete myChessAI;
	myChessAI = nullptr;
}

void CGameWorld::Init(Client *aClient)
{
	if (aClient && !myPlayingLocally)
	{
		myClient = aClient;
		myClient->myGameWorld = this;
		myClient->Start();
	}
	if (myPlayingWithAI)
	{
		myChessAI = new ChessAI();
		myChessAI->myGameWorld = this;
	}

	CreateBoard();
}

void CGameWorld::CreateBoard()
{
	Rook *whiteRook0 = nullptr;
	Rook *whiteRook1 = nullptr;
	Rook *rook0 = nullptr;
	Rook *rook1 = nullptr;

	myBoard.mySquares.resize(8);
	for (auto i = 0; i < 8; i++)
	{
		myBoard.mySquares[i].resize(8);
		for (auto j = 0; j < 8; j++)
		{
			myBoard.mySquares[i][j] = new Square(VECTOR2I(i,j), myBoard.mySquares);
			if (myBoardIsFlipped)
			{
				VECTOR2F worldPos = { 0.175f + (myBoardIsFlipped ? (7 - i) : i) * SQUARE_SIZE, 0.125f + (myBoardIsFlipped ? (7 - j) : j) * SQUARE_SIZE };
				myBoard.mySquares[i][j]->mySprite->SetPosition(worldPos);
				if (Piece *piece = myBoard.mySquares[i][j]->myPiece)
				{
					piece->mySprite->SetPosition(worldPos);
				}
			}

			if (Piece* piece = myBoard.mySquares[i][j]->myPiece)
			{
				myBoard.myPieces.push_back(piece);
				myBoard.myPieces.back()->myGameWorld = this;
				myBoard.myPieces.back()->myPieceIndex = myBoard.myPieces.size() - 1;

				if (myBoard.myPieces.back()->myIsWhite)
				{
					myBoard.myWhitePieces.emplace_back(piece);
				}
				else
				{
					myBoard.myBlackPieces.emplace_back(piece);
				}

				if (Rook* castedRook = dynamic_cast<Rook *>(myBoard.myPieces.back()))
				{
					if (castedRook->myIsWhite ? !whiteRook0 : !rook0)
					{
						castedRook->myIsWhite ? whiteRook0 = castedRook : rook0 = castedRook;
					}
					else
					{
						castedRook->myIsWhite ? whiteRook1 = castedRook : rook1 = castedRook;
					}
				}
				if (King* castedKing = dynamic_cast<King *>(myBoard.myPieces.back()))
				{
					castedKing->myIsWhite ? myBoard.myWhiteKing = castedKing : myBoard.myBlackKing = castedKing;
				}
			}
		}
	}

	myBoard.myWhiteKing->myRooks = std::make_pair(whiteRook0, whiteRook1);
	myBoard.myBlackKing->myRooks = std::make_pair(rook0, rook1);
}

void CGameWorld::OnRelease(const VECTOR2F &aPos)
{
	float shortestDistance = 10000.0f;
	if (myPickedUpPiece != -1)
	{
		Square *selectedSquare = nullptr;
		for (int i = 0; i < myBoard.mySquares.size(); i++)
		{
			for (int j = 0; j < myBoard.mySquares[i].size(); j++)
			{
				VECTOR2F diff = myBoard.mySquares[i][j]->mySprite->GetPosition() - aPos;
				float distance = sqrt((diff.x * diff.x) + (diff.y * diff.y));
				if (distance < shortestDistance && distance != -1.0f)
				{
					shortestDistance = distance;
					selectedSquare = myBoard.mySquares[i][j];
				}
			}
		}

		if (selectedSquare && selectedSquare->myPiece != myBoard.myPieces[myPickedUpPiece])
		{
			Square *fromSquare = myBoard.myPieces[myPickedUpPiece]->mySquare;
			if (myBoard.myPieces[myPickedUpPiece]->OnRelease(selectedSquare))
			{
				myBoard.myIsWhitesTurn = !myBoard.myIsWhitesTurn;
				CheckIfCheckmateOnBoard(&myBoard);
				SendMove(fromSquare->myPosition, selectedSquare->myPosition);
				if (myChessAI)
				{
					if (myShouldThread)
					{
						std::thread aiThread([=]() { myChessAI->FindBestMove(myBoard); });
						aiThread.detach();
					}
					else
					{
						std::pair<VECTOR2I, VECTOR2I> move = myChessAI->FindBestMove(myBoard).GetAsVector();
						MakeMove(move.first, move.second);
					}
				}
			}
			myPickedUpPiece = -1;
			return;
		}
	}
}

void CGameWorld::RemovePiece(int aIndex)
{
	myBoard.myPieces[aIndex]->mySquare->myPiece = nullptr;

	std::iter_swap(myBoard.myPieces.begin() + aIndex, myBoard.myPieces.end() - 1);
	delete myBoard.myPieces.back();
	myBoard.myPieces.pop_back();

	for (auto i = 0; i < myBoard.myPieces.size(); i++)
	{
		myBoard.myPieces[i]->myPieceIndex = i;
	}
}

void CGameWorld::SetLastMove(const VECTOR2I &aFrom, const VECTOR2I &aTo)
{
	if (myLastMove.first.x != -1)
	{
		if ((myLastMove.first.x + myLastMove.first.y) % 2 == 1)
		{
			myBoard.mySquares[myLastMove.first.x][myLastMove.first.y]->mySprite->SetColor(GREEN_COLOR);
		}
		else
		{
			myBoard.mySquares[myLastMove.first.x][myLastMove.first.y]->mySprite->SetColor(OFF_WHITE_COLOR);
		}
		if ((myLastMove.second.x + myLastMove.second.y) % 2 == 1)
		{
			myBoard.mySquares[myLastMove.second.x][myLastMove.second.y]->mySprite->SetColor(GREEN_COLOR);
		}
		else
		{
			myBoard.mySquares[myLastMove.second.x][myLastMove.second.y]->mySprite->SetColor(OFF_WHITE_COLOR);
		}
	}

	myLastMove = std::make_pair(aFrom, aTo);

	if ((aFrom.x + aFrom.y) % 2 == 1)
	{
		myBoard.mySquares[aFrom.x][aFrom.y]->mySprite->SetColor({ 0.7f,1.0f,0.2f,1.0f });
	}
	else
	{
		myBoard.mySquares[aFrom.x][aFrom.y]->mySprite->SetColor({ 1.0f,1.0f,0.575f,1.0f });
	}
	if ((aTo.x + aTo.y) % 2 == 1)
	{
		myBoard.mySquares[aTo.x][aTo.y]->mySprite->SetColor({ 0.7f,1.0f,0.2f,1.0f });
	}
	else
	{
		myBoard.mySquares[aTo.x][aTo.y]->mySprite->SetColor({ 1.0f,1.0f,0.575f,1.0f });
	}
}

bool CGameWorld::IsInCheck(bool aIsWhite, Board* aBoard)
{
	King *king = aIsWhite ? aBoard->myWhiteKing : aBoard->myBlackKing;
	bool kingHasMoved = king->myHasMoved;
	for (auto &piece : aBoard->myPieces)
	{
		if (piece->myIsWhite == aIsWhite || !piece->mySquare)
		{
			continue;
		}

		king->myHasMoved = true;
		if (piece->IsValidMove(piece->mySquare, king->mySquare))
		{
			king->myHasMoved = kingHasMoved;
			return true;
		}
	}

	king->myHasMoved = kingHasMoved;
	return false;
}

bool CGameWorld::IsInCheckAfter(Piece *aLastPiece, Square *aSquare, Board *aBoard)
{
	if (!aLastPiece->IsValidMove(aLastPiece->mySquare, aSquare))
	{
		return true;
	}

	Piece *initialPiece = aLastPiece;
	Square *initialSquare = aLastPiece->mySquare;
	Piece *initialPieceAtSquare = aSquare->myPiece;

	aLastPiece->mySquare->myPiece = nullptr;
	aLastPiece->mySquare = aSquare;
	if (aSquare->myPiece)
	{
		aSquare->myPiece->mySquare = nullptr;
	}
	aSquare->myPiece = aLastPiece;

	if (IsInCheck(aLastPiece->myIsWhite, aBoard))
	{
		return true;
	}

	initialPiece->mySquare = initialSquare;
	initialSquare->myPiece = initialPiece;
	aSquare->myPiece = initialPieceAtSquare;
	if (initialPieceAtSquare)
	{
		aSquare->myPiece->mySquare = aSquare;
	}
	return false;
}

bool CGameWorld::CheckIfCheckmateOnBoard(Board *aBoard)
{
	King *king = aBoard->myIsWhitesTurn ? aBoard->myWhiteKing : aBoard->myBlackKing;
	bool kingHasMoved = king->myHasMoved;

	if (!king->mySquare)
	{
		return true;
	}

	VECTOR2I pos = king->mySquare->myPosition;

	if (!IsInCheck(aBoard->myIsWhitesTurn, &myBoard))
	{
		return false;
	}

	for (auto& piece : myBoard.myPieces)
	{
		for (auto& squareVector : myBoard.mySquares)
		{
			for (auto &square : squareVector)
			{
				if (piece->myIsWhite != aBoard->myIsWhitesTurn || piece->mySquare == square)
				{
					continue;
				}

				king->myHasMoved = true;
				if ((!square->myPiece|| square->myPiece->myIsWhite != king->myIsWhite) && !IsInCheckAfter(piece, square, &myBoard))
				{
					king->myHasMoved = kingHasMoved;
					return false;
				}
			}
		}
	}
	king->myHasMoved = kingHasMoved;
	aBoard->myCheckmate = true;
	return true;
}

void CGameWorld::OnClick(const VECTOR2F& aPos)
{
	float shortestDistance = 10000.0f;

	if (myPickedUpPiece != -1)
	{
		Square *selectedSquare = nullptr;
		for (int i = 0; i < myBoard.mySquares.size(); i++)
		{
			for (int j = 0; j < myBoard.mySquares[i].size(); j++)
			{
				VECTOR2F diff = myBoard.mySquares[i][j]->mySprite->GetPosition() - aPos;
				float distance = sqrt((diff.x * diff.x) + (diff.y * diff.y));
				if (distance < shortestDistance && distance != -1.0f)
				{
					shortestDistance = distance;
					selectedSquare = myBoard.mySquares[i][j];
				}
			}
		}

		Square *fromSquare = myBoard.myPieces[myPickedUpPiece]->mySquare;
		if (myBoard.myPieces[myPickedUpPiece]->OnRelease(selectedSquare))
		{
			myBoard.myIsWhitesTurn = !myBoard.myIsWhitesTurn;
			CheckIfCheckmateOnBoard(&myBoard);
			SendMove(fromSquare->myPosition, selectedSquare->myPosition);
			if (myChessAI)
			{
				if (myShouldThread)
				{
					std::thread aiThread([=]() { myChessAI->FindBestMove(myBoard); });
					aiThread.detach();
				}
				else
				{
					std::pair<VECTOR2I, VECTOR2I> move = myChessAI->FindBestMove(myBoard).GetAsVector();
					MakeMove(move.first, move.second);
				}
			}
		}
		myPickedUpPiece = -1;
		return;
	}

	if (myBoard.myCheckmate)
	{
		return;
	}

	for (int i = 0; i < myBoard.myPieces.size(); i++)
	{
		float distance = myBoard.myPieces[i]->OnClick(aPos);
		if (distance < shortestDistance && distance != -1.0f)
		{
			shortestDistance = distance;
			myPickedUpPiece = i;
		}
	}
}

void CGameWorld::SendMove(const VECTOR2I &aFrom, const VECTOR2I &aTo)
{
	if (!myClient)
	{
		return;
	}
	char buffer[DEFAULT_BUFLEN];
	CNetMessageVector2i message;
	message.Set2Vector2i({ aFrom.x, aFrom.y }, { aTo.x, aTo.y });
	memcpy(buffer, &message, sizeof(CNetMessageVector2i));
	myClient->SendBytes(buffer);
}

void CGameWorld::MakeMove(const VECTOR2I &aFrom, const VECTOR2I &aTo)
{
	Square *from = myBoard.mySquares[aFrom.x][aFrom.y];
	Square *to = myBoard.mySquares[aTo.x][aTo.y];
	Piece *piece = myBoard.mySquares[aFrom.x][aFrom.y]->myPiece;

	if (!piece)
	{
		std::cout << typeid(*piece).name() << "tried to get piece from x:" << from->myPosition.x << ", y:"
			<< ", but there is no piece there...\n";
		return;
	}

	if (!piece->IsValidMove(from, to))
	{
		std::cout << typeid(*piece).name() << "tried to move from x:" << from->myPosition.x << ", y:" 
			<< from->myPosition.y << " to x:" << to->myPosition.x << ", y:" << to->myPosition.y << "\n";
		return;
	}

	if (to->myPiece)
	{
		RemovePiece(to->myPiece->myPieceIndex);
	}

	SetLastMove(aFrom, aTo);

	from->myPiece = nullptr;
	to->myPiece = piece;
	piece->mySquare = to;
	piece->mySprite->SetPosition(to->mySprite->GetPosition());
	piece->OnMoved();

	myBoard.myIsWhitesTurn = !myBoard.myIsWhitesTurn;
	CheckIfCheckmateOnBoard(&myBoard);
}

void CGameWorld::FlipBoard()
{
	myBoardIsFlipped = !myBoardIsFlipped;

	for (auto i = 0; i < myBoard.mySquares.size(); i++)
	{
		for (auto j = 0; j < myBoard.mySquares[i].size(); j++)
		{
			VECTOR2F worldPos = { 0.175f + (myBoardIsFlipped ? (7 - i) : i) * SQUARE_SIZE, 0.125f + (myBoardIsFlipped ? (7 - j) : j) * SQUARE_SIZE };
			myBoard.mySquares[i][j]->mySprite->SetPosition(worldPos);
			if (Piece *piece = myBoard.mySquares[i][j]->myPiece)
			{
				piece->mySprite->SetPosition(worldPos);
			}
		}
	}
}

void CGameWorld::Update(float aDeltaTime)
{
	static bool fIsDown = false;
	if (GetAsyncKeyState('F') && !fIsDown)
	{
		fIsDown = true;
		FlipBoard();
	}
	
	if (!GetAsyncKeyState('F'))
	{
		fIsDown = false;
	}

	if (myPickedUpPiece != -1)
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(*Tga2D::CEngine::GetInstance()->GetHWND(), &p);
		float x = static_cast<float>(p.x) / static_cast<float>(Tga2D::CEngine::GetInstance()->GetWindowSize().x);
		float y = static_cast<float>(p.y) / static_cast<float>(Tga2D::CEngine::GetInstance()->GetWindowSize().y);
		myBoard.myPieces[myPickedUpPiece]->mySprite->SetPosition({ x, y });
	}

	for (auto i = 0; i < myBoard.mySquares.size(); i++)
	{
		for (auto j = 0; j < myBoard.mySquares.size(); j++)
		{
			if (myBoard.mySquares[i][j]->mySprite)
			{
				myBoard.mySquares[i][j]->mySprite->Render();
			}
		}
	}

	for (auto i = 0; i < myBoard.myPieces.size(); i++)
	{
		if (myPickedUpPiece == i)
		{
			continue;
		}
		myBoard.myPieces[i]->Render();
	}

	if (myPickedUpPiece != -1)
	{
		myBoard.myPieces[myPickedUpPiece]->Render();
	}

	if (GetAsyncKeyState(VK_ESCAPE))
	{
		Tga2D::CEngine::Shutdown();
	}
}
