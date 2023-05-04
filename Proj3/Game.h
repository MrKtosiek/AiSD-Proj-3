#pragma once
#include <iostream>
#include "HexPos.h"
#include "Move.h"
#include "Capture.h"
#include "Vector.h"

#define TILE_EMPTY '_'
#define TILE_WHITE 'W'
#define TILE_BLACK 'B'

class Game
{
public:
	int size = 0;
	char** tiles = nullptr;
	char activePlayer = TILE_WHITE;
	int whiteReserve = 0;
	int blackReserve = 0;
	int* activePlayerReserve = &whiteReserve;
	int* inactivePlayerReserve = &blackReserve;
	size_t maxChain = 0;
	enum class GameState
	{
		IN_PROGRESS,
		BAD_MOVE,
		WHITE_WIN,
		BLACK_WIN,
		DEAD_LOCK
	} gameState = GameState::IN_PROGRESS;

	Move lastMove = {};

	Vector<Capture> possibleCaptures;


	Game(){}
	Game(const Game& orig)
	{
		size = orig.size;
		activePlayer = orig.activePlayer;
		whiteReserve = orig.whiteReserve;
		blackReserve = orig.blackReserve;
		maxChain = orig.maxChain;

		SetPlayerReserves();

		int rowCount = GetRowCount();
		tiles = new char* [rowCount];
		for (int i = 0; i < rowCount; i++)
		{
			tiles[i] = new char[rowCount]();
			for (int j = 0; j < GetRowSize(i); j++)
				tiles[i][j + GetRowOffset(i)] = orig.tiles[i][j + GetRowOffset(i)];
		}
	}
	Game(int size, char activePlayer, int whitePieces, int blackPieces, int whiteReserve, int blackReserve, size_t maxChain)
		: size(size), activePlayer(activePlayer), whiteReserve(whiteReserve), blackReserve(blackReserve), maxChain(maxChain)
	{
		SetPlayerReserves();

		int rowCount = GetRowCount();
		tiles = new char* [rowCount];
		for (int i = 0; i < rowCount; i++)
		{
			tiles[i] = new char[rowCount]();
			for (int j = 0; j < GetRowSize(i); j++)
				tiles[i][j + GetRowOffset(i)] = TILE_EMPTY;
		}
	}
	~Game()
	{
		if (tiles != nullptr)
		{
			for (int i = 0; i < GetRowCount(); i++)
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
		return GetRowCount() - abs((int)(row - size + 1));
	}
	int GetRowCount() const
	{
		return 2 * size - 1;
	}

	void SwitchPlayer()
	{
		if (activePlayer == TILE_WHITE)
			activePlayer = TILE_BLACK;
		else
			activePlayer = TILE_WHITE;

		SetPlayerReserves();
	}
	void SetPlayerReserves()
	{
		if (activePlayer == TILE_WHITE)
		{
			activePlayerReserve = &whiteReserve;
			inactivePlayerReserve = &blackReserve;
		}
		else
		{
			activePlayerReserve = &blackReserve;
			inactivePlayerReserve = &whiteReserve;
		}
	}

	void ReadState()
	{
		for (int i = 0; i < GetRowCount(); i++)
		{
			for (int j = 0; j < GetRowSize(i); j++)
				std::cin >> tiles[i][j + GetRowOffset(i)];
		}
	}
	void PrintBoard()
	{
		std::cout << "White reserve: " << whiteReserve << "\n";
		std::cout << "Black reserve: " << blackReserve << "\n";

		for (int i = 0; i < GetRowCount(); i++)
		{
			for (int x = 0; x < abs((int)(i - size + 1)); x++)
				std::cout << ' ';

			for (int j = 0; j < GetRowSize(i); j++)
				std::cout << tiles[i][j + GetRowOffset(i)] << ' ';
			std::cout << '\n';
		}
	}

	void DoMove(const Move move)
	{
		if (gameState == GameState::WHITE_WIN || gameState == GameState::BLACK_WIN || gameState == GameState::DEAD_LOCK)
		{
			// the game is finished
			return;
		}

		lastMove = move;
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

		CheckPossibleCaptures(move);
		for (size_t i = 0; i < possibleCaptures.GetLength(); i++)
		{
			std::cout << "Before capture " << i << "\n";
			PrintBoard();
			CapturePieces(possibleCaptures[i]);
			std::cout << "After capture " << i << "\n";
			PrintBoard();
		}

		SwitchPlayer();


		// check winning conditions
		if (*activePlayerReserve <= 0)
		{
			if (activePlayer == TILE_WHITE)
				gameState = GameState::BLACK_WIN;
			else
				gameState = GameState::WHITE_WIN;

			return;
		}

		if (GetLegalMoves().GetLength() == 0)
		{
			gameState = GameState::DEAD_LOCK;
			return;
		}


		gameState = GameState::IN_PROGRESS;
	}

	void CheckPossibleCaptures(Move move)
	{
		possibleCaptures = Vector<Capture>();

		// check crossing captures
		HexPos cur = move.to;
		size_t dir = move.from.GetNeighborIndex(move.to);
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			Capture cap = { cur, cur.GetNeighbor(dir + 1) - cur, tiles[cur.x][cur.y] };
			
			if (CheckCapture(cap))
				possibleCaptures.Append(cap);

			cap.dir = cur.GetNeighbor(dir + 2) - cur;
			
			if (CheckCapture(cap))
				possibleCaptures.Append(cap);

			cur = cur.GetNeighbor(dir);
		}

		// check parallel captures
		cur = move.to;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			Capture cap = { cur, cur.GetNeighbor(dir) - cur, tiles[cur.x][cur.y] };

			if (CheckCapture(cap))
				possibleCaptures.Append(cap);

			// skip the captured chain
			while (IsOnBoard(cur) && tiles[cur.x][cur.y] == cap.color)
			{
				cur = cur.GetNeighbor(dir);
			}
		}

		OrderPossibleCaptures();
	}
	void OrderPossibleCaptures()
	{
		for (size_t i = 0; i < possibleCaptures.GetLength(); i++)
		{
			for (size_t j = 0; j < i; j++)
			{
				if (possibleCaptures[j].color != activePlayer && possibleCaptures[j + 1].color == activePlayer)
				{
					Capture tmp = possibleCaptures[j];
					possibleCaptures[j] = possibleCaptures[j + 1];
					possibleCaptures[j + 1] = tmp;
				}
			}
		}
	}

	bool CheckCapture(Capture cap)
	{
		int row = 1;

		HexPos cur = cap.pos + cap.dir;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] == cap.color)
		{
			row++;
			cur += cap.dir;
		}
		cur = cap.pos - cap.dir;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] == cap.color)
		{
			row++;
			cur -= cap.dir;
		}

		return (row >= maxChain);
	}

	void CapturePieces(Capture cap)
	{
		HexPos cur = cap.pos;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			if (tiles[cur.x][cur.y] == cap.color)
			{
				if (cap.color == activePlayer)
					*activePlayerReserve += 1;
				else
					*inactivePlayerReserve += 1;
			}

			tiles[cur.x][cur.y] = TILE_EMPTY;
			cur += cap.dir;
		}
		cur = cap.pos - cap.dir;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			if (tiles[cur.x][cur.y] == cap.color)
			{
				if (cap.color == activePlayer)
					*activePlayerReserve += 1;
				else
					*inactivePlayerReserve += 1;
			}

			tiles[cur.x][cur.y] = TILE_EMPTY;
			cur -= cap.dir;
		}
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


	//     Notation:                       How it's stored                               
	//                                                                                   
	//      1 2 3 4 5 6 7 8 9              -1 0 1 2 3 4 5 6 7                            
	//                                                                 Tile neighbors:   
	//  a   + + + + +                  -1   + + + + +                                    
	//  b   + W _ _ B +                 0   + W _ _ B +                4 5               
	//  c   + _ _ _ _ _ +               1   + _ _ _ _ _ +              3 T 0             
	//  d   + _ _ _ _ _ _ +             2   + _ _ _ _ _ _ +              2 1             
	//  e   + B _ _ _ _ _ W +           3   + B _ _ _ _ _ W +                            
	//  f   + _ _ _ _ _ _ +             4     + _ _ _ _ _ _ +                            
	//  g   + _ _ _ _ _ +               5       + _ _ _ _ _ +                            
	//  h   + W _ _ B +                 6         + W _ _ B +                            
	//  i   + + + + +                   7           + + + + +                            
	//                                                                                   
	//      1 2 3 4 5 6 7 8 9              -1 0 1 2 3 4 5 6 7                            

	HexPos NotationToHex(const String& str) const
	{
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
			pos.x < GetRowCount() && pos.y < GetRowCount() &&
			pos.x - pos.y < size && pos.y - pos.x < size);
	}
	bool IsMoveLegal(Move move) const
	{
		// initial checks
		if (IsOnBoard(move.from) || !IsOnBoard(move.to))
		{
			//std::cout << "incorrect coords\n";
			return false;
		}
		if (!move.to.IsNeighbor(move.from))
		{
			//std::cout << "not neighbors\n";
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
		//std::cout << "no space\n";
		return false;
	}

	Vector<Move> GetLegalMoves()
	{
		Vector<Move> moves;

		HexPos cur = { -1,-1 };
		for (int i = 0; i < 6; i++)
		{
			for (int n = 0; n < size; n++)
			{
				Move move = { cur, cur.GetNeighbor(i + 1) };

				if (IsMoveLegal(move))
					moves.Append(move);

				move.to = cur.GetNeighbor(i + 2);

				if (IsMoveLegal(move))
					moves.Append(move);

				cur = cur.GetNeighbor(i);
			}
		}

		return moves;
	}


	Game& operator=(const Game& other)
	{
		Game tmp(other);

		activePlayer = tmp.activePlayer;
		whiteReserve = tmp.whiteReserve;
		blackReserve = tmp.blackReserve;
		maxChain = tmp.maxChain;

		SetPlayerReserves();

		int s = tmp.size;
		tmp.size = size;
		size = s;

		char** t = tmp.tiles;
		tmp.tiles = tiles;
		tiles = t;
		return *this;
	}
};

