#pragma once
#include "Game.h"
#include "GamePosition.h"
#include "Vector.h"

class Solver
{
private:
	Game* game = nullptr;
public:
	Solver(Game* game) : game(game) {}
	
	void PlayRandomMove()
	{
		Vector<Move> legalMoves = game->GetLegalMoves();
		if (legalMoves.GetLength() > 0)
			game->DoMove(legalMoves[rand() % legalMoves.GetLength()]);
	}
};
