#pragma once
#include "Game.h"
#include "GamePosition.h"
#include "Vector.h"

#include <algorithm>

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


	int EvaluateGame(size_t depth)
	{
		size_t evaluatedNodes = 0;

		int eval = Minimax(depth, -GamePosition::MAX_EVAL, GamePosition::MAX_EVAL, &evaluatedNodes);
		
		std::cout << "Evaluated:" << evaluatedNodes << '\n';
		return eval;
	}

	int Minimax(size_t depth, int alpha, int beta, size_t* evaluatedNodes)
	{
		GamePosition gamePos = game->GetGamePosition();

		if (depth == 0 || game->gameState != Game::GameState::IN_PROGRESS)
		{
			return gamePos.EvaluatePosition();
		}

		Vector<Move> moves = game->GetLegalMoves();

		*evaluatedNodes += 1;

		if (game->activePlayer == game->WHITE)
		{
			int value = -GamePosition::MAX_EVAL;

			for (size_t i = 0; i < moves.GetLength(); i++)
			{
				game->DoMove(moves[i]);
				int childEval = Minimax(depth - 1, alpha, beta, evaluatedNodes);
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

			for (size_t i = 0; i < moves.GetLength(); i++)
			{
				game->DoMove(moves[i]);
				int childEval = Minimax(depth - 1, alpha, beta, evaluatedNodes);
				game->RestorePosition(gamePos);

				value = std::min(value, childEval);

				if (USE_ALPHA_BETA && value < alpha)
					break;
				beta = std::min(beta, value);
			}

			return value;
		}
	}

	int Negamax(size_t depth, int alpha, int beta, int color, size_t* evaluatedNodes)
	{
		GamePosition gamePos = game->GetGamePosition();
		
		if (depth == 0 || game->gameState != Game::GameState::IN_PROGRESS)
		{
			return gamePos.EvaluatePosition() * color;
		}

		*evaluatedNodes += 1;

		Vector<Move> moves = game->GetLegalMoves();

		// create all child nodes
		Vector<GamePosition> childNodes;
		for (size_t i = 0; i < moves.GetLength(); i++)
		{
			game->DoMove(moves[i]);
			GamePosition child = game->GetGamePosition();
			game->RestorePosition(gamePos);

			childNodes.Append(child);
		}

		// bubblesort child nodes by their evaluation, descending
		for (size_t i = 0; i < childNodes.GetLength() - 1; i++)
			for (size_t j = 0; j < childNodes.GetLength() - 1 - i; j++)
				if (childNodes[j].evaluation * color > childNodes[j + 1].evaluation * color)
					std::swap(childNodes[j], childNodes[j + 1]);

		int value = -GamePosition::MAX_EVAL;

		for (size_t i = 0; i < childNodes.GetLength(); i++)
		{
			game->RestorePosition(childNodes[i]);
			int childEval = -Negamax(depth - 1, -beta, -alpha, -color, evaluatedNodes);
			value = std::max(value, childEval);
			alpha = std::max(alpha, value);
			if (alpha >= beta)
				break;
		}

		return value;
	}
};
