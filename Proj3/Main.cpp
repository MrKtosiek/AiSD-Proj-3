#include <iostream>
#include <ctime>
#include "Game.h"
#include "HexPos.h"
#include "Solver.h"

#include <crtdbg.h>

using namespace std;



void InputGame(Game& game)
{
	size_t size;
	size_t maxChain;
	size_t whitePieces;
	size_t blackPieces;
	size_t whiteReserve;
	size_t blackReserve;
	char activePlayer;

	cin >> size;
	cin >> maxChain;
	cin >> whitePieces;
	cin >> blackPieces;
	cin >> whiteReserve;
	cin >> blackReserve;
	cin >> activePlayer;

	game = Game(size, activePlayer, whitePieces, blackPieces, whiteReserve, blackReserve, maxChain);

	game.ReadState();
}

Move ReadMove(const Game& game)
{
	String fromNotation;
	String toNotation;
	std::cin >> fromNotation;
	std::cin >> toNotation;
	Move move;
	move.from = game.NotationToHex(fromNotation);
	move.to = game.NotationToHex(toNotation);
	return move;
}


void Program()
{
	Game game;
	Solver solver(&game);

	String input;
	while (true)
	{
		cin >> input;
		input.Capitalize();

		if (input == "QUIT")
		{
			return;
		}
		else if (input == "LOAD_GAME_BOARD")
		{
			InputGame(game);
		}
		else if (input == "PRINT_GAME_BOARD")
		{
			game.PrintBoard();
		}
		else if (input == "DO_MOVE")
		{
			Move move = ReadMove(game);
			game.DoMove(move);
		}
		else if (input == "PRINT_GAME_STATE")
		{
			game.PrintGameState();
		}
		else if (input == "GEN_ALL_POS_MOV")
		{
			Vector<Move> moves = game.GetLegalMoves();
			for (size_t i = 0; i < moves.GetLength(); i++)
			{
				cout << game.HexToNotation(moves[i].from) << " " << game.HexToNotation(moves[i].to) << "\n";
			}
		}
		else if (input == "GEN_ALL_POS_MOV_NUM")
		{
			Vector<Move> moves = game.GetLegalMoves();
			cout << moves.GetLength() << "\n";
		}
		else if (input == "PLAY_RANDOM")
		{
			solver.PlayRandomMove();
		}
	}
}

int main()
{
	//srand(time(nullptr));

	// wait for input
	while (cin.peek() == EOF) {}
	int start = clock();

	Program();

	// measure time
	int end = clock();
	cout << "Finished after "<< (double)(end - start)/CLOCKS_PER_SEC << "s\n";

	_CrtDumpMemoryLeaks();
	return 0;
}
