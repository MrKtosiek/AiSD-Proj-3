#pragma once
#include <iostream>
#include "HexPos.h"
#include "Move.h"

#define TILE_EMPTY '_'
#define TILE_WHITE 'W'
#define TILE_BLACK 'B'

class Game
{
private:
	char** tiles = nullptr;
	char activePlayer = TILE_WHITE;
	int whiteReserve = 0;
	int blackReserve = 0;
	int* activePlayerReserve = &whiteReserve;
	enum class GameState
	{
		IN_PROGRESS,
		BAD_MOVE,
		WHITE_WIN,
		BLACK_WIN,
		DEAD_LOCK
	} gameState = GameState::IN_PROGRESS;

	Move lastMove;

public:
	int size = 0;

	Game(){}
	Game(const Game& orig)
	{
		size = orig.size;
		activePlayer = orig.activePlayer;
		whiteReserve = orig.whiteReserve;
		blackReserve = orig.blackReserve;

		if (activePlayer == TILE_WHITE)
			activePlayerReserve = &whiteReserve;
		else
			activePlayerReserve = &blackReserve;

		tiles = new char* [size * 2 - 1];
		for (int i = 0; i < size * 2 - 1; i++)
		{
			tiles[i] = new char[size * 2 - 1]();
			for (int j = 0; j < GetRowSize(i); j++)
				tiles[i][j + GetRowOffset(i)] = orig.tiles[i][j + GetRowOffset(i)];
		}
	}
	Game(int size, char activePlayer, int whitePieces, int blackPieces, int whiteReserve, int blackReserve)
		: size(size), activePlayer(activePlayer), whiteReserve(whiteReserve), blackReserve(blackReserve)
	{
		if (activePlayer == TILE_WHITE)
			activePlayerReserve = &whiteReserve;
		else
			activePlayerReserve = &blackReserve;

		tiles = new char* [size * 2 - 1];
		for (int i = 0; i < size * 2 - 1; i++)
		{
			tiles[i] = new char[size * 2 - 1]();
			for (int j = 0; j < GetRowSize(i); j++)
				tiles[i][j + GetRowOffset(i)] = TILE_EMPTY;
		}
	}
	~Game()
	{
		if (tiles != nullptr)
		{
			for (int i = 0; i < size * 2 - 1; i++)
				delete[] tiles[i];
			delete[] tiles;
		}
	}

	int GetRowOffset(int row) const
	{
		if (row >= size)
			return row - size + 1;
		return 0;
	}
	int GetRowSize(int row) const
	{
		return 2 * size - 1 - abs((int)(row - size + 1));
	}
	int GetRowCount(int row) const
	{
		return 2 * size - 1;
	}

	void SwitchPlayer()
	{
		if (activePlayer == TILE_WHITE)
		{
			activePlayer = TILE_BLACK;
			activePlayerReserve = &blackReserve;
		}
		else
		{
			activePlayer = TILE_WHITE;
			activePlayerReserve = &whiteReserve;
		}
	}

	void ReadState()
	{
		for (int i = 0; i < size * 2 - 1; i++)
		{
			for (int j = 0; j < GetRowSize(i); j++)
				std::cin >> tiles[i][j + GetRowOffset(i)];
		}
	}
	void PrintBoard()
	{
		std::cout << "White reserve: " << whiteReserve << "\n";
		std::cout << "Black reserve: " << blackReserve << "\n";

		for (int i = 0; i < size * 2 - 1; i++)
		{
			for (int x = 0; x < abs((int)(i - size + 1)); x++)
				std::cout << ' ';

			for (int j = 0; j < GetRowSize(i); j++)
				std::cout << tiles[i][j + GetRowOffset(i)] << ' ';
			std::cout << '\n';
		}
	}

	void DoMove(Move move)
	{
		std::cout << "doing move\n";
		lastMove = move;

		if (*activePlayerReserve <= 0)
		{
			gameState = GameState::DEAD_LOCK;
			return;
		}

		if (!IsMoveLegal(move))
		{
			gameState = GameState::BAD_MOVE;
			return;
		}

		// move the pieces
		HexPos cur = move.to;
		HexPos dir = move.to - move.from;
		char valToMove = activePlayer;
		while (tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			char tmp = tiles[cur.x][cur.y];
			tiles[cur.x][cur.y] = valToMove;
			valToMove = tmp;

			cur += dir;
		}
		tiles[cur.x][cur.y] = valToMove;

		
		(*activePlayerReserve)--;

		SwitchPlayer();
	}

	void PrintGameState()
	{
		switch (gameState)
		{
		case GameState::IN_PROGRESS:
			std::cout << "in_progress\n";
			break;
		case GameState::BAD_MOVE:
			std::cout << "bad_move " << activePlayer << ' ' << HexToNotation(lastMove.from) << ' ' << HexToNotation(lastMove.to) << "\n";
			break;
		case GameState::WHITE_WIN:
			std::cout << "white_win\n";
			break;
		case GameState::BLACK_WIN:
			std::cout << "black_win\n";
			break;
		case GameState::DEAD_LOCK:
			std::cout << "dead_lock " << activePlayer << "\n";
			break;
		default:
			break;
		}
	}

	HexPos NotationToHex(const String& str) const
	{
		//     Notation:                       How it's stored
		// 
		//      1 2 3 4 5 6 7 8 9              -1 0 1 2 3 4 5 6 7     
		//                                                            
		//  a   + + + + +                  -1   + + + + +             
		//  b   + W _ _ B +                 0   + W _ _ B +           
		//  c   + _ _ _ _ _ +               1   + _ _ _ _ _ +         
		//  d   + _ _ _ _ _ _ +             2   + _ _ _ _ _ _ +       
		//  e   + B _ _ _ _ _ W +           3   + B _ _ _ _ _ W +     
		//  f   + _ _ _ _ _ _ +             4     + _ _ _ _ _ _ +     
		//  g   + _ _ _ _ _ +               5       + _ _ _ _ _ +     
		//  h   + W _ _ B +                 6         + W _ _ B +     
		//  i   + + + + +                   7           + + + + +     
		//                                                            
		//      1 2 3 4 5 6 7 8 9              -1 0 1 2 3 4 5 6 7     


		int row = str[0] - 'b';
		int col = 0;

		for (int i = 1; str[i] != 0; i++)
		{
			col *= 10;
			char digit = str[i];
			if (digit >= '0' && digit <= '9')
			{
				col += digit - '0';
			}
		}

		col -= 2;
		if (row >= size)
			col += row - size + 1;

		return HexPos(row, col);
	}
	String HexToNotation(const HexPos& hex) const
	{
		String str;
		String colStr;
		str.Append(hex.x + 'b');

		int col = hex.y + 2;
		if (hex.x >= size)
			col -= hex.x - size + 1;

		colStr = col;
		str.Append(colStr);

		return str;
	}

	bool IsOnBoard(HexPos pos) const
	{
		return
			(pos.x >= 0 && pos.y >= 0 &&
			pos.x < size * 2 - 1 && pos.y < size * 2 - 1 &&
			pos.x - pos.y < size && pos.y - pos.x < size);
	}
	bool IsMoveLegal(Move move) const
	{
		// initial checks
		if (IsOnBoard(move.from) || !IsOnBoard(move.to))
		{
			std::cout << "incorrect coords\n";
			return false;
		}
		if (!move.to.IsNeighbor(move.from))
		{
			std::cout << "not neighbors\n";
			return false;
		}

		// check if the pieces can be pushed
		HexPos cur = move.to;
		HexPos dir = move.to - move.from;
		while (IsOnBoard(cur))
		{
			if (tiles[cur.x][cur.y] == TILE_EMPTY)
				return true;

			cur += dir;
		}
		return false;
	}


	Game& operator=(const Game& other)
	{
		Game tmp(other);
		size = tmp.size;
		activePlayer = tmp.activePlayer;
		whiteReserve = tmp.whiteReserve;
		blackReserve = tmp.blackReserve;

		if (activePlayer == TILE_WHITE)
			activePlayerReserve = &whiteReserve;
		else
			activePlayerReserve = &blackReserve;
		
		char** t = tmp.tiles;
		tmp.tiles = tiles;
		tiles = t;
		return *this;
	}
};

