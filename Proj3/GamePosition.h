#pragma once
#include "String.h"
#include "Game.h"

struct GamePosition
{
	String board;
	int whitePieces = 0;
	int blackPieces = 0;
	int whiteReserve = 0;
	int blackReserve = 0;

	GamePosition() {}
	GamePosition(const Game& game)
		: whiteReserve(game.whiteReserve), blackReserve(game.blackReserve)
	{
		whitePieces = 0;
		blackPieces = 0;

		for (int x = 0; x < game.GetRowCount(); x++)
		{
			for (int y = 0; y < game.GetRowSize(x); y++)
			{
				board.Append(game.tiles[x][y + game.GetRowOffset(x)]);
				switch (game.tiles[x][y + game.GetRowOffset(x)])
				{
				case 'W':
					whitePieces++;
					break;
				case 'B':
					blackPieces++;
					break;
				default:
					break;
				}
			}
		}
	}

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
