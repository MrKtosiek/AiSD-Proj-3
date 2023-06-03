#pragma once
#include "Move.h"
#include "GameState.h"
#include <string>

struct GamePosition
{
	std::string board;
	int activePlayer = 0;
	int whitePieces = 0;
	int blackPieces = 0;
	int whiteReserve = 0;
	int blackReserve = 0;
	GameState gameState = GameState::INCORRECT_BOARD;

	Move lastMove;

	static const int MAX_EVAL = 1000000000;

	int evaluation = 0;

	int EvaluatePosition() const
	{
		if (gameState == GameState::DEAD_LOCK)
		{
			if (activePlayer == 0)
				return -MAX_EVAL;
			else
				return MAX_EVAL;
		}

		if (gameState == GameState::WHITE_WIN)
			return MAX_EVAL;
		if (gameState == GameState::BLACK_WIN)
			return -MAX_EVAL;

		int eval = 0;
		eval += whitePieces + whiteReserve;
		eval -= blackPieces + blackReserve;

		return eval;
	}


	bool operator==(const GamePosition& other) const
	{
		return
			activePlayer == other.activePlayer &&
			gameState == other.gameState &&
			whitePieces == other.whitePieces &&
			blackPieces == other.blackPieces &&
			whiteReserve == other.whiteReserve &&
			blackReserve == other.blackReserve &&
			board == other.board;
	}
};

template <> struct std::hash<GamePosition>
{
	size_t operator()(const GamePosition& pos) const
	{
		size_t hash = 0;
		size_t p = 1;
		for (size_t i = 0; i < pos.board.length(); i++)
		{
			hash += pos.board[i] * p;
			p *= 3; // only 3 possible characters, 'W' 'B' '_'
		}
		return hash;
	}
};
