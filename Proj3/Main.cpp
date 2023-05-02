#include <iostream>
#include "Board.h"
#include "HexPos.h"

#include <crtdbg.h>

using namespace std;



void InputGame(Board& board)
{
	size_t size;
	size_t maxChain;
	size_t whitePieces;
	size_t blackPieces;
	size_t whiteReserve;
	size_t blackReserve;
	char playerToMove;

	cin >> size;
	cin >> maxChain;
	cin >> whitePieces;
	cin >> blackPieces;
	cin >> whiteReserve;
	cin >> blackReserve;
	cin >> playerToMove;

	board = Board(size);

	board.ReadState();
}



void Program()
{
	Board board;

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
			InputGame(board);
		}
		else if (input == "PRINT_GAME_BOARD")
		{
			board.WriteState();
		}
		else if (input == "DO_MOVE")
		{
			board.DoMove();
		}
	}

}

int main()
{
	Program();
	_CrtDumpMemoryLeaks();
	return 0;
}
