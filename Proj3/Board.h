#pragma once
#include <iostream>
#include "HexPos.h"

#define TILE_EMPTY '_'
#define TILE_WHITE 'W'
#define TILE_BLACK 'B'

class Board
{
private:
	char** tiles = nullptr;
	char activePlayer = TILE_WHITE;

public:
	int size = 0;

	Board(){}
	Board(const Board& orig)
	{
		this->size = orig.size;
		tiles = new char* [size * 2 - 1];
		for (int i = 0; i < size * 2 - 1; i++)
		{
			tiles[i] = new char[size * 2 - 1]();
			for (int j = 0; j < GetRowSize(i); j++)
				tiles[i][j + GetRowOffset(i)] = orig.tiles[i][j + GetRowOffset(i)];
		}
	}
	Board(int size)
	{
		this->size = size;
		tiles = new char* [size * 2 - 1];
		for (int i = 0; i < size * 2 - 1; i++)
		{
			tiles[i] = new char[size * 2 - 1]();
			for (int j = 0; j < GetRowSize(i); j++)
				tiles[i][j + GetRowOffset(i)] = TILE_EMPTY;
		}
	}
	~Board()
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

	void ReadState()
	{
		for (int i = 0; i < size * 2 - 1; i++)
		{
			for (int j = 0; j < GetRowSize(i); j++)
				std::cin >> tiles[i][j + GetRowOffset(i)];
		}
	}
	void WriteState()
	{
		for (int i = 0; i < size * 2 - 1; i++)
		{
			for (int x = 0; x < abs((int)(i - size + 1)); x++)
				std::cout << ' ';

			for (int j = 0; j < GetRowSize(i); j++)
				std::cout << tiles[i][j + GetRowOffset(i)] << ' ';
			std::cout << '\n';
		}
	}

	void DoMove()
	{
		std::cout << "doing move\n";
		String fromNotation;
		String toNotation;
		std::cin >> fromNotation;
		std::cin >> toNotation;
		HexPos from = NotationToHex(fromNotation);
		HexPos to = NotationToHex(toNotation);

		if (!IsMoveLegal(from, to))
		{
			std::cout << "move illegal\n";
			return;
		}

		// move the pieces
		HexPos cur = to;
		HexPos dir = to - from;
		char valToMove = activePlayer;
		while (tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			char tmp = tiles[cur.x][cur.y];
			tiles[cur.x][cur.y] = valToMove;
			valToMove = tmp;

			cur += dir;
		}
		tiles[cur.x][cur.y] = valToMove;
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

	bool IsOnBoard(HexPos pos) const
	{
		return
			(pos.x >= 0 && pos.y >= 0 &&
			pos.x < size * 2 - 1 && pos.y < size * 2 - 1 &&
			pos.x - pos.y < size && pos.y - pos.x < size);
	}
	bool IsMoveLegal(HexPos from, HexPos to) const
	{
		// initial checks
		if (IsOnBoard(from) || !IsOnBoard(to))
		{
			std::cout << "incorrect coords\n";
			return false;
		}
		if (!to.IsNeighbor(from))
		{
			std::cout << "not neighbors\n";
			return false;
		}

		// check if the pieces can be pushed
		HexPos cur = to;
		HexPos dir = to - from;
		while (IsOnBoard(cur))
		{
			if (tiles[cur.x][cur.y] == TILE_EMPTY)
				return true;

			cur += dir;
		}
		return false;
	}


	Board& operator=(const Board& other)
	{
		Board tmp(other);
		size = tmp.size;
		char** t = tmp.tiles;
		tmp.tiles = tiles;
		tiles = t;
		return *this;
	}
};

