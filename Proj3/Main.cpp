#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "Game.h"
#include "HexPos.h"
#include "Solver.h"

#include <crtdbg.h>

using namespace std;

int clockStart = 0;


// reads a string from cin until it reaches one of the endChars
// doesn't remove the endChar from cin
void ReadString(std::string& str, const std::string& endChars)
{
	std::string tmp;

	cin >> ws;

	while (cin && endChars.find(cin.peek()) == std::string::npos)
	{
		char input = getchar();
		if (!cin.eof())
			tmp.push_back(input);
	}
	str = tmp;
}

void CapitalizeString(std::string& str)
{
	for (size_t i = 0; i < str.length(); i++)
	{
		str[i] = toupper(str[i]);
	}
}


void InputGame(Game& game)
{
	size_t size;
	size_t rowCaptureLength;
	size_t whiteMaxPieces;
	size_t blackMaxPieces;
	size_t whiteReserve;
	size_t blackReserve;
	char activePlayer;

	cin >> size;
	cin >> rowCaptureLength;
	cin >> whiteMaxPieces;
	cin >> blackMaxPieces;
	cin >> whiteReserve;
	cin >> blackReserve;
	cin >> activePlayer;

	game = Game(size, activePlayer, whiteMaxPieces, blackMaxPieces, whiteReserve, blackReserve, rowCaptureLength);

	game.ReadBoard();
}

void ReadMove(Game& game)
{
	std::string fromNotation;
	ReadString(fromNotation, "-");
	cin.get(); // skip the '-'
	std::string toNotation;
	ReadString(toNotation, " \n");

	Move move = { game.NotationToHex(fromNotation), game.NotationToHex(toNotation) };

	while (cin.peek() != '\n')
	{
		std::string color;
		ReadString(color, " ");
		
		Capture cap;
		if (color == "w:")
			cap.player = game.WHITE;
		else
			cap.player = game.BLACK;

		std::string start;
		std::string end;

		ReadString(start, " ");
		ReadString(end, "\n");

		cap.start = game.NotationToHex(start);
		cap.end = game.NotationToHex(end);

		move.captures.push_back(cap);
	}
	
	if (game.DoMove(move))
		cout << "MOVE_COMMITTED\n";
}


void Program()
{
	Game game;
	Solver solver(&game);

	std::string input;
	while (!cin.eof())
	{
		ReadString(input, " \n");
		CapitalizeString(input);

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
		else if (input == "IS_GAME_OVER")
		{
			switch (game.gameState)
			{
			case GameState::IN_PROGRESS:
				cout << "GAME_IN_PROGRESS\n";
				break;
			case GameState::WHITE_WIN:
				cout << "THE_WINNER_IS_WHITE\n";
				break;
			case GameState::BLACK_WIN:
				cout << "THE_WINNER_IS_BLACK\n";
				break;
			case GameState::DEAD_LOCK:
				if (game.activePlayer == game.WHITE)
					cout << "THE_WINNER_IS_BLACK\n";
				else
					cout << "THE_WINNER_IS_WHITE\n";
				break;
			default:
				break;
			}
		}
		else if (input == "GEN_ALL_POS_MOV")
		{
			std::vector<Move> moves = game.GetLegalMoves();
			GamePosition gamePos = game.GetGamePosition();
			for (size_t i = 0; i < moves.size(); i++)
			{
				cout << game.MoveToNotation(moves[i]) << "\n";
				//game.DoMove(moves[i]);
				//game.PrintBoard();
				//game.RestorePosition(gamePos);
			}
		}
		else if (input == "GEN_ALL_POS_MOV_NUM")
		{
			std::vector<Move> moves = game.GetLegalMoves();
			cout << moves.size() << "_UNIQUE_MOVES\n";
		}
		else if (input == "GEN_ALL_POS_MOV_EXT")
		{
			std::vector<Move> moves = game.GetLegalMoves();
			GamePosition gamePos = game.GetGamePosition();

			int eval = solver.EvaluateGame(2);
			if ((game.activePlayer == game.WHITE && eval == GamePosition::MAX_EVAL) ||
				(game.activePlayer == game.BLACK && eval == -GamePosition::MAX_EVAL))
			{
				cout << game.MoveToNotation(moves[0]) << "\n";
				game.DoMove(moves[0]);
				game.PrintBoard();
				game.RestorePosition(gamePos);
			}
			else
			{
				for (size_t i = 0; i < moves.size(); i++)
				{
					cout << game.MoveToNotation(moves[i]) << "\n";
					game.DoMove(moves[i]);
					game.PrintBoard();
					game.RestorePosition(gamePos);
				}
			}
		}
		else if (input == "GEN_ALL_POS_MOV_EXT_NUM")
		{
			std::vector<Move> moves = game.GetLegalMoves();
			int eval = solver.EvaluateGame(2);
			if (eval == GamePosition::MAX_EVAL || eval == -GamePosition::MAX_EVAL)
				cout << "1_UNIQUE_MOVES\n";
			else
				cout << moves.size() << "_UNIQUE_MOVES\n";
		}
		else if (input == "SOLVE_GAME_STATE")
		{
			size_t depthLimit = 20;
			int eval = solver.EvaluateGame(depthLimit);

			if (eval == GamePosition::MAX_EVAL)
				cout << "WHITE_HAS_WINNING_STRATEGY\n";
			else if (eval == -GamePosition::MAX_EVAL)
				cout << "BLACK_HAS_WINNING_STRATEGY\n";
		}
		else if (input == "WINNING_SEQUENCE_EXIST")
		{
			size_t depth;
			cin >> depth;
			int eval = solver.EvaluateGame(depth);

			if (eval == GamePosition::MAX_EVAL)
				cout << "WHITE_HAS_WINNING_STRATEGY\n";
			else if (eval == -GamePosition::MAX_EVAL)
				cout << "BLACK_HAS_WINNING_STRATEGY\n";
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
	srand(time(nullptr));

	// wait for input
	while (cin.peek() == EOF) {}
	clockStart = clock();

	Program();

	_CrtDumpMemoryLeaks();
	return 0;
}
