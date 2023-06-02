#pragma once
#include "String.h"
#include "Move.h"


struct GamePosition
{
	String board;
	int activePlayer = 0;
	int whitePieces = 0;
	int blackPieces = 0;
	int whiteReserve = 0;
	int blackReserve = 0;
	int gameState = 0;

	int rowCaptureLength = 0;
	// numbers of rows of length k-1 and k-2
	int whiteRowNumbers[2] = {};
	int blackRowNumbers[2] = {};
	
	Move lastMove;

	static const int MAX_EVAL = 1000000000;

	int evaluation = 0;

	GamePosition() {}

	int EvaluatePosition() const
	{
		if (blackReserve == 0)
			return MAX_EVAL;
		else if (whiteReserve == 0)
			return -MAX_EVAL;

		int eval = 0;
		eval += whitePieces * 2 + whiteReserve;
		eval -= blackPieces * 2 + blackReserve;

		for (size_t i = 0; i < 2; i++)
		{
			eval += whiteRowNumbers[i] * (rowCaptureLength - 1 - i);
			eval -= blackRowNumbers[i] * (rowCaptureLength - 1 - i);
		}

		return eval;
	}


	bool operator==(const GamePosition& other) const
	{
		return
			gameState == other.gameState &&
			whitePieces == other.whitePieces &&
			blackPieces == other.blackPieces &&
			whiteReserve == other.whiteReserve &&
			blackReserve == other.blackReserve &&
			board == other.board;
	}
};
