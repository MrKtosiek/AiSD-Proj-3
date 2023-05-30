#pragma once
#include "Game.h"
#include "GamePosition.h"
#include "Vector.h"

class Solver
{
private:
	static const bool USE_ALPHA_BETA = true;

	Game* game = nullptr;
public:
	explicit Solver(Game* game) : game(game) {}
	
	void PlayRandomMove()
	{
		Vector<Move> legalMoves = game->GetLegalMoves();
		if (legalMoves.GetLength() > 0)
		{
			Move move = legalMoves[rand() % legalMoves.GetLength()];
			std::cout << "playing " << game->MoveToNotation(move) << "\n";
			game->DoMove(move);
			return;
		}
		else
		{
			std::cout << "no legal moves found\n";
		}
	}



	int Minimax(size_t depth, int alpha, int beta)
	{
		GamePosition gamePos = game->GetGamePosition();

		if (depth == 0 || game->gameState != Game::GameState::IN_PROGRESS)
		{
			return gamePos.Evaluate();
		}

		if (game->activePlayer == game->WHITE)
		{
			int value = -GamePosition::MAX_EVAL;

			Vector<Move> moves = game->GetLegalMoves();

			for (size_t i = 0; i < moves.GetLength(); i++)
			{
				game->DoMove(moves[i]);
				int childEval = Minimax(depth - 1, alpha, beta);
				game->RestorePosition(gamePos);
				value = std::max(value, childEval);

				if (USE_ALPHA_BETA && value > beta)
					break;
				alpha = std::max(alpha, value);
			}

			return value;
		}
		else
		{
			int value = GamePosition::MAX_EVAL;

			Vector<Move> moves = game->GetLegalMoves();

			for (size_t i = 0; i < moves.GetLength(); i++)
			{
				game->DoMove(moves[i]);
				int childEval = Minimax(depth - 1, alpha, beta);
				game->RestorePosition(gamePos);
				value = std::min(value, childEval);

				if (USE_ALPHA_BETA && value < alpha)
					break;
				beta = std::min(beta, value);
			}

			return value;
		}
	}
};
