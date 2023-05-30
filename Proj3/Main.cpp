//#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <ctime>
#include "Game.h"
#include "HexPos.h"
#include "Solver.h"

#include <crtdbg.h>

using namespace std;

int clockStart = 0;


// reads a string from cin until it reaches one of the endChars
// doesn't remove the endChar from cin
void ReadString(String& str, const String& endChars)
{
	String tmp;

	cin >> ws;

	while (cin && !endChars.Contains(cin.peek()))
	{
		char input = getchar();
		if (!cin.eof())
			tmp.Append(input);
	}
	str = tmp;
}


void InputGame(Game& game)
{
	size_t size;
	size_t maxChain;
	size_t whiteMaxPieces;
	size_t blackMaxPieces;
	size_t whiteReserve;
	size_t blackReserve;
	char activePlayer;

	cin >> size;
	cin >> maxChain;
	cin >> whiteMaxPieces;
	cin >> blackMaxPieces;
	cin >> whiteReserve;
	cin >> blackReserve;
	cin >> activePlayer;

	game = Game(size, activePlayer, whiteMaxPieces, blackMaxPieces, whiteReserve, blackReserve, maxChain);

	game.ReadBoard();
}

void ReadMove(Game& game)
{
	String fromNotation;
	ReadString(fromNotation, "-");
	cin.get(); // skip the '-'
	String toNotation;
	ReadString(toNotation, " \n");

	Move move = { game.NotationToHex(fromNotation), game.NotationToHex(toNotation) };

	while (cin.peek() != '\n')
	{
		String color;
		ReadString(color, " ");
		
		Capture cap;
		if (color == "w:")
			cap.player = game.WHITE;
		else
			cap.player = game.BLACK;

		String start;
		String end;

		ReadString(start, " ");
		ReadString(end, "\n");

		cap.start = game.NotationToHex(start);
		cap.end = game.NotationToHex(end);

		move.captures.Append(cap);
	}
	
	if (game.DoMove(move))
		cout << "MOVE_COMMITTED\n";
}


void Program()
{
	Game game;
	Solver solver(&game);

	String input;
	while (!cin.eof())
	{
		ReadString(input, " \n");
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
			ReadMove(game);
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
				cout << game.MoveToNotation(moves[i]) << "\n";
			}
		}
		else if (input == "GEN_ALL_POS_MOV_NUM")
		{
			Vector<Move> moves = game.GetLegalMoves();
			cout << moves.GetLength() << "_UNIQUE_MOVES\n";
		}
		else if (input == "WINNING_SEQUENCE_EXIST")
		{
			size_t depth;
			cin >> depth;
			int eval = solver.Minimax(depth, -GamePosition::MAX_EVAL, GamePosition::MAX_EVAL);
			cout << eval << "\n";
		}
		else if (input == "RESET_CLOCK")
		{
			clockStart = clock();
		}
		else if (input == "PRINT_TIME")
		{
			cout << "Time: " << (double)(clock() - clockStart) / CLOCKS_PER_SEC << "s\n";
		}
		else if (input == "PLAY_RANDOM")
		{
			solver.PlayRandomMove();
		}
	}
}


int main()
{
	//freopen("output.txt", "w", stdout);
	srand(time(nullptr));

	// wait for input
	while (cin.peek() == EOF) {}
	clockStart = clock();

	Program();

	_CrtDumpMemoryLeaks();
	return 0;
}
