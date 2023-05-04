#pragma once
#include "Game.h"
#include "GamePosition.h"
#include "Vector.h"

class Solver
{
private:

public:

	Vector<Move> GenerateLegalMoves(const Game& game) const
	{
		Vector<Move> moves;

		HexPos cur = { -1,-1 };
		for (int i = 0; i < 6; i++)
		{
			for (int n = 0; n < game.size; n++)
			{
				Move move = { cur, cur.GetNeighbor(i + 1) };

				if (game.IsMoveLegal(move))
					moves.Append(move);

				move.to = cur.GetNeighbor(i + 2);

				if (game.IsMoveLegal(move))
					moves.Append(move);
				
				cur = cur.GetNeighbor(i);
			}
		}

		return moves;
	}
};
