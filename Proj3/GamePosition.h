#pragma once
#include "String.h"

struct GamePosition
{
	String board;
	int activePlayer = 0;
	int whitePieces = 0;
	int blackPieces = 0;
	int whiteReserve = 0;
	int blackReserve = 0;
	int gameState = 0;

	GamePosition() {}

	int Evaluate() const
	{
		return (whitePieces * 2 + whiteReserve) - (blackPieces * 2 + blackReserve);
	}

	bool operator==(const GamePosition& other)
	{
		return
			whitePieces == other.whitePieces &&
			blackPieces == other.blackPieces &&
			whiteReserve == other.whiteReserve &&
			blackReserve == other.blackReserve &&
			board == other.board;
	}
};
