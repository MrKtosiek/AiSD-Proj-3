#pragma once
#include "Game.h"
#include "GamePosition.h"

#include <vector>
#include <algorithm>
#include <unordered_map>

class Solver
{
private:
	static const bool USE_ALPHA_BETA = true;
	size_t visited = 0;
	size_t pruned = 0;
	size_t skippedWithTable = 0;

	std::unordered_map<GamePosition, int> transpositionTable;

	Game* game = nullptr;
public:
	explicit Solver(Game* game) : game(game) {}
	
	void PlayRandomMove()
	{
		std::vector<Move> legalMoves = game->GetLegalMoves();
		if (legalMoves.size() > 0)
		{
			Move move = legalMoves[rand() % legalMoves.size()];
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
		visited = 0;
		pruned = 0;
		skippedWithTable = 0;

		int eval = Minimax(depth, -GamePosition::MAX_EVAL, GamePosition::MAX_EVAL);
		
		//std::cout << "Visited:" << visited << '\n';
		//std::cout << "Pruned:" << pruned << '\n';
		//std::cout << "Skipped:" << skippedWithTable << '\n';

		transpositionTable.clear();
		return eval;
	}

	int Minimax(size_t depth, int alpha, int beta)
	{
		GamePosition gamePos = game->GetGamePosition();

		visited++;

		if (depth == 0 || game->gameState != GameState::IN_PROGRESS)
		{
			return gamePos.EvaluatePosition();
		}

		std::vector<Move> moves = game->GetLegalMoves();

		if (game->activePlayer == game->WHITE)
		{
			int valueMax = -GamePosition::MAX_EVAL;

			for (size_t i = 0; i < moves.size(); i++)
			{
				game->DoMove(moves[i]);
				GamePosition childNode = game->GetGamePosition();
				int childEval;
				auto result = transpositionTable.find(childNode);
				if (result == transpositionTable.end())
				{
					childEval = Minimax(depth - 1, alpha, beta);
					transpositionTable.insert({ childNode, childEval });
				}
				else
				{
					skippedWithTable++;
					childEval = result->second;
				}
				
				game->RestorePosition(gamePos);

				valueMax = std::max(valueMax, childEval);

				alpha = std::max(alpha, valueMax);
				if (alpha >= beta)
				{
					pruned++;
					break;
				}
			}
			return valueMax;
		}
		else
		{
			int valueMin = GamePosition::MAX_EVAL;

			for (size_t i = 0; i < moves.size(); i++)
			{
				game->DoMove(moves[i]);
				int childEval = Minimax(depth - 1, alpha, beta);
				game->RestorePosition(gamePos);

				valueMin = std::min(valueMin, childEval);

				beta = std::min(beta, valueMin);
				if (beta <= alpha)
				{
					pruned++;
					break;
				}
			}
			return valueMin;
		}
	}
};
