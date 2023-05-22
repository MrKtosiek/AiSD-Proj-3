#pragma once
#include <iostream>
#include "HexPos.h"
#include "Move.h"
#include "Capture.h"
#include "Vector.h"
#include "GamePosition.h"
#include "GamePositionSet.h"


class Game
{
public:

	const int WHITE = 0;
	const int BLACK = 1;
	const char TILE_EMPTY = '_';
	const char TILE_WHITE = 'W';
	const char TILE_BLACK = 'B';

	int size = 0;
	char** tiles = nullptr;
	int activePlayer = 0;
	const char playerColors[2] = { TILE_WHITE, TILE_BLACK };
	int playerReserves[2] = { 0, 0 };
	int playerPieces[2] = { 0, 0 };
	int playerMaxPieces[2] = { 0, 0 };
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
	Move lastBadMove = {};


	Game(){}
	Game(const Game& orig)
	{
		size = orig.size;
		maxChain = orig.maxChain;
		activePlayer = orig.activePlayer;
		playerReserves[WHITE] = orig.playerReserves[WHITE];
		playerReserves[BLACK] = orig.playerReserves[BLACK];
		playerPieces[WHITE] = orig.playerPieces[WHITE];
		playerPieces[BLACK] = orig.playerPieces[BLACK];
		playerMaxPieces[WHITE] = orig.playerMaxPieces[WHITE];
		playerMaxPieces[BLACK] = orig.playerMaxPieces[BLACK];

		int rowCount = GetRowCount();
		tiles = new char* [rowCount];
		for (int i = 0; i < rowCount; i++)
		{
			tiles[i] = new char[rowCount]();
			for (int j = 0; j < GetRowSize(i); j++)
				tiles[i][j + GetRowOffset(i)] = orig.tiles[i][j + GetRowOffset(i)];
		}
	}
	Game(int size, char activePlayer, int whiteMaxPieces, int blackMaxPieces, int whiteReserve, int blackReserve, size_t maxChain)
		: size(size), maxChain(maxChain)
	{
		if (activePlayer == TILE_WHITE)
			this->activePlayer = WHITE;
		else
			this->activePlayer = BLACK;

		playerReserves[WHITE] = whiteReserve;
		playerReserves[BLACK] = blackReserve;
		playerMaxPieces[WHITE] = whiteMaxPieces;
		playerMaxPieces[BLACK] = blackMaxPieces;

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
		if (row < size)
			return size - row - 1;
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
	HexPos GetCenter() const
	{
		return { size - 1, size - 1 };
	}

	void SwitchPlayer()
	{
		activePlayer = InactivePlayer();
	}
	int InactivePlayer() const
	{
		return (activePlayer + 1) % 2;
	}

	void ReadBoard()
	{
		std::cin >> std::ws;
		String board;
		size_t correctBoardLength = 0;

		for (int i = 0; i < GetRowCount(); i++)
		{
			correctBoardLength += GetRowSize(i);

			char* line = new char[256];
			std::cin.getline(line, 256);

			for (size_t l = 0; l < strlen(line); l++)
			{
				char c = line[l];
				if (c == TILE_EMPTY || c == TILE_WHITE || c == TILE_BLACK)
				{
					board.Append(c);
					if (c == TILE_WHITE)
						playerPieces[WHITE] += 1;
					if (c == TILE_BLACK)
						playerPieces[BLACK] += 1;
				}
			}

			delete[] line;
		}


		if (board.GetLength() != correctBoardLength)
		{
			std::cout << "WRONG_BOARD_ROW_LENGTH\n";
			return;
		}
		if (playerMaxPieces[WHITE] - playerPieces[WHITE] != playerReserves[WHITE])
		{
			std::cout << "WRONG_WHITE_PAWNS_NUMBER\n";
			return;
		}
		if (playerMaxPieces[BLACK] - playerPieces[BLACK] != playerReserves[BLACK])
		{
			std::cout << "WRONG_BLACK_PAWNS_NUMBER\n";
			return;
		}


		int cursor = 0;
		for (int i = 0; i < GetRowCount(); i++)
		{
			for (int j = 0; j < GetRowSize(i); j++)
			{
				HexPos center = { size - 1, size - 1 };
				HexPos pos = { i, j + GetRowOffset(i) };
				pos -= center;
				pos = pos.RotateLeft();
				pos += center;
				tiles[pos.x][pos.y] = board[cursor];
				cursor++;
			}
		}


		std::cout << "BOARD_STATE_OK\n";
		return;
	}
	void PrintBoard() const
	{
		std::cout << size << " ";
		std::cout << maxChain << " ";
		std::cout << playerMaxPieces[WHITE] << " ";
		std::cout << playerMaxPieces[BLACK] << "\n";
		std::cout << playerReserves[WHITE] << " ";
		std::cout << playerReserves[BLACK] << " ";
		std::cout << playerColors[activePlayer] << "\n";

		for (int i = 0; i < GetRowCount(); i++)
		{
			for (int x = 0; x < abs((int)(i - size + 1)); x++)
				std::cout << ' ';

			for (int j = 0; j < GetRowSize(i); j++)
			{
				HexPos center = { size - 1, size - 1 };
				HexPos pos = { i, j + GetRowOffset(i) };
				pos -= center;
				pos = pos.RotateLeft();
				pos += center;
				std::cout << tiles[pos.x][pos.y] << ' ';
			}
			std::cout << '\n';
		}
	}

	void DoMove(const Move& move)
	{
		if (gameState == GameState::WHITE_WIN || gameState == GameState::BLACK_WIN || gameState == GameState::DEAD_LOCK)
		{
			// the game has finished
			return;
		}


		// check if the move is legal and if there are no unsettled captures
		if (!IsMoveLegal(move))
		{
			std::cout << "Illegal move\n";
			lastBadMove = move;
			gameState = GameState::BAD_MOVE;
			return;
		}
		Vector<Capture> possibleCaptures = GetPossibleCaptures(activePlayer);
		if (possibleCaptures.GetLength() > 1)
		{
			std::cout << "Can't move, there are captures that need to be resolved\n";
			lastBadMove = move;
			gameState = GameState::BAD_MOVE;
			return;
		}


		// check game ending conditions
		if (playerReserves[activePlayer] <= 0)
		{
			if (activePlayer == WHITE)
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

		// move the pieces
		Push(move);

		ResolveCaptures(move);

		lastMove = move;
		gameState = GameState::IN_PROGRESS;
	}
	void DoMove(const Capture& capture)
	{
		if (gameState == GameState::WHITE_WIN || gameState == GameState::BLACK_WIN || gameState == GameState::DEAD_LOCK)
		{
			// the game has finished
			return;
		}


		// verify the capture notation
		if (capture.player != activePlayer)
		{
			std::cout << "wrong player color\n";
			gameState = GameState::BAD_MOVE;
			return;
		}
		if (!CheckCapture(capture))
		{
			std::cout << "incorrect capture\n";
			gameState = GameState::BAD_MOVE;
			return;
		}

		CapturePieces(capture);

		ResolveCaptures(lastMove);
	}

	// automatically perform all captures that don't need any decisions, and switch the player
	void ResolveCaptures(const Move& previousMove)
	{
		Vector<Capture> possibleCaptures = GetPossibleCaptures(activePlayer);
		if (possibleCaptures.GetLength() == 1)
		{
			CapturePieces(possibleCaptures[0]);
		}
		if (possibleCaptures.GetLength() <= 1)
		{
			SwitchPlayer();

			possibleCaptures = GetPossibleCaptures(activePlayer);
			if (possibleCaptures.GetLength() == 1)
			{
				CapturePieces(possibleCaptures[0]);
			}
		}
	}

	void Push(const Move& move)
	{
		HexPos cur = move.to;
		HexPos dir = move.to - move.from;
		char valToMove = playerColors[activePlayer];
		while (tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			char tmp = tiles[cur.x][cur.y];
			tiles[cur.x][cur.y] = valToMove;
			valToMove = tmp;

			cur += dir;
		}
		tiles[cur.x][cur.y] = valToMove;

		playerReserves[activePlayer] -= 1;
	}
	

	// scans the entire board for chains long enough to capture
	Vector<Capture> GetPossibleCaptures(int player) const
	{
		Vector<Capture> possibleCaptures;

		HexPos edgePos = { -1, size - 1 };

		for (int i = 0; i < 6; i++)
		{
			for (int n = 0; n < size; n++)
			{
				// prevent the diagonals from being checked 2 times from different sides
				if (n == 0 && i % 2 == 0)
				{
					edgePos = edgePos.GetNeighbor(i);
					continue;
				}

				HexPos cur = edgePos.GetNeighbor(i + 1);
				
				while (IsOnBoard(cur))
				{
					Capture cap(cur, cur.GetNeighbor(i + 1) - cur, player);

					if (CheckCapture(cap))
					{
						possibleCaptures.Append(cap);

						// skip the captured pieces
						while (IsOnBoard(cur) && tiles[cur.x][cur.y] == playerColors[player])
						{
							cur = cur.GetNeighbor(i + 1);
						}
					}

					cur = cur.GetNeighbor(i + 1);
				}

				edgePos = edgePos.GetNeighbor(i);
			}
		}

		return possibleCaptures;
	}

	// faster than the version without the 'move' argument, 
	// but only checks for captures that could have been caused by the passed move
	Vector<Capture> GetPossibleCaptures(Move move, int player) const
	{
		Vector<Capture> possibleCaptures;

		// check crossing captures
		HexPos cur = move.to;
		size_t dir = move.from.GetNeighborIndex(move.to);
		while (IsOnBoard(cur))
		{
			if (tiles[cur.x][cur.y] == playerColors[player])
			{
				Capture cap(cur, cur.GetNeighbor(dir + 1) - cur, player);

				if (CheckCapture(cap))
					possibleCaptures.Append(cap);

				cap.dir = cur.GetNeighbor(dir + 2) - cur;

				if (CheckCapture(cap))
					possibleCaptures.Append(cap);
			}

			cur = cur.GetNeighbor(dir);
		}

		// check parallel captures
		cur = move.to;
		while (IsOnBoard(cur))
		{
			if (tiles[cur.x][cur.y] == playerColors[player])
			{
				Capture cap(cur, cur.GetNeighbor(dir) - cur, player);

				if (CheckCapture(cap))
					possibleCaptures.Append(cap);

				// skip the captured chain
				while (IsOnBoard(cur) && tiles[cur.x][cur.y] == playerColors[cap.player])
				{
					cur = cur.GetNeighbor(dir);
				}
			}
			cur = cur.GetNeighbor(dir);
		}

		return possibleCaptures;
	}


	bool CheckCapture(const Capture& cap) const
	{
		size_t row = 1;

		if (tiles[cap.pos.x][cap.pos.y] != playerColors[cap.player])
			return false;

		HexPos cur = cap.pos + cap.dir;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] == playerColors[cap.player])
		{
			row++;
			cur += cap.dir;
		}
		cur = cap.pos - cap.dir;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] == playerColors[cap.player])
		{
			row++;
			cur -= cap.dir;
		}

		return (row >= maxChain);
	}

	void CapturePieces(const Capture& cap)
	{
		HexPos cur = cap.pos;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			CapturePiece(cur, cap.player);
			cur += cap.dir;
		}
		cur = cap.pos - cap.dir;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			CapturePiece(cur, cap.player);
			cur -= cap.dir;
		}
	}
	void CapturePiece(HexPos pos, int capturer)
	{
		if (tiles[pos.x][pos.y] == playerColors[capturer])
		{
			if (capturer == activePlayer)
				playerReserves[activePlayer] += 1;
			else
				playerReserves[InactivePlayer()] += 1;
		}

		playerPieces[CharToPlayer(tiles[pos.x][pos.y])] -= 1;
		tiles[pos.x][pos.y] = TILE_EMPTY;
	}

	bool NotationToCapture(const Vector<HexPos>& pieces, Capture& capture) const
	{
		if (pieces.GetLength() < maxChain) // the chain is too short
		{
			return false;
		}

		if (!pieces[0].IsNeighbor(pieces[1]))
		{
			return false;
		}

		HexPos cur = pieces[0];
		HexPos dir = pieces[1] - pieces[0];
		for (size_t i = 0; i < pieces.GetLength(); i++)
		{
			if (!(pieces[i] == cur))
			{
				return false;
			}

			cur += dir;
		}

		capture.pos = pieces[0];
		capture.dir = dir;
		return true;
	}
	String CaptureToNotation(const Capture& capture)
	{
		String result;

		HexPos cur = capture.pos;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] == playerColors[capture.player])
		{
			cur -= capture.dir;
		}
		cur += capture.dir;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] == playerColors[capture.player])
		{
			result.Append(HexToNotation(cur));
			result.Append(' ');
			cur += capture.dir;
		}

		return result;
	}

	void PrintGameState() const
	{
		switch (gameState)
		{
		case GameState::IN_PROGRESS:
			std::cout << "in_progress\n";
			break;
		case GameState::BAD_MOVE:
			std::cout << "bad_move " << playerColors[activePlayer];
			std::cout << ' ' << HexToNotation(lastBadMove.from) << ' ' << HexToNotation(lastBadMove.to) << "\n";
			break;
		case GameState::WHITE_WIN:
			std::cout << "white_win\n";
			break;
		case GameState::BLACK_WIN:
			std::cout << "black_win\n";
			break;
		case GameState::DEAD_LOCK:
			std::cout << "dead_lock " << playerColors[activePlayer] << "\n";
			break;
		default:
			break;
		}
	}


	//              How it's stored                                   Notation:               
	//                                                                                        
	//              -1 0 1 2 3 4 5 6 7                                   5 6 7 8 9            
	//                                          Tile neighbors:         4 \ \ \ \ \           
	//          -1           + + + + +                                 3 \ + + + + + - 9      
	//           0         + B W W W +            4 5                 2 \ + W _ _ B + - 8     
	//           1       + _ _ _ _ _ +          3 T 0                1 \ + W _ _ B _ + - 7    
	//           2     + _ _ _ _ _ _ +          2 1                   \ + W _ _ B _ _ + - 6   
	//           3   + W W W _ B B B +                                 + B _ _ _ _ _ W + - 5  
	//           4   + _ _ _ _ _ _ +                                  / + _ _ W _ _ B + - 4   
	//           5   + _ _ _ _ _ +                                   a / + _ W _ _ B + - 3    
	//           6   + B B B W +                                      b / + W _ _ B + - 2     
	//           7   + + + + +                                         c / + + + + + - 1      
	//                                                                  d / / / / /           
	//              -1 0 1 2 3 4 5 6 7                                   e f g h i            
	//                                                                                        
	//                                                                                        


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
		if (row < size)
			col += size - row - 1;

		return HexPos(row, col);
	}
	String HexToNotation(const HexPos& hex) const
	{
		String str;
		String colStr;
		str.Append(hex.x + 'b');

		int col = hex.y + 2;
		if (hex.x < size)
			col -= size - hex.x - 1;

		colStr = col;
		str.Append(colStr);

		return str;
	}

	int CharToPlayer(char c) const
	{
		return (c == TILE_WHITE) ? WHITE : BLACK;
	}

	GamePosition GetGamePosition() const
	{
		GamePosition gamePos;
		gamePos.activePlayer = activePlayer;
		gamePos.whiteReserve = playerReserves[WHITE];
		gamePos.blackReserve = playerReserves[BLACK];
		gamePos.whitePieces = playerPieces[WHITE];
		gamePos.blackPieces = playerPieces[BLACK];
		gamePos.gameState = (int)gameState;
		gamePos.lastMove = lastMove;

		for (int x = 0; x < GetRowCount(); x++)
		{
			for (int y = 0; y < GetRowSize(x); y++)
			{
				gamePos.board.Append(tiles[x][y + GetRowOffset(x)]);
			}
		}

		return gamePos;
	}
	void RestorePosition(const GamePosition& gamePos)
	{
		activePlayer = gamePos.activePlayer;
		playerReserves[WHITE] = gamePos.whiteReserve;
		playerReserves[BLACK] = gamePos.blackReserve;
		playerPieces[WHITE] = gamePos.whitePieces;
		playerPieces[BLACK] = gamePos.blackPieces;
		gameState = (GameState)gamePos.gameState;
		lastMove = gamePos.lastMove;

		int c = 0;
		for (int i = 0; i < GetRowCount(); i++)
		{
			for (int j = 0; j < GetRowSize(i); j++)
			{
				tiles[i][j + GetRowOffset(i)] = gamePos.board[c];
				c++;
			}
		}
	}

	bool IsOnBoard(HexPos pos) const
	{
		return
			(pos.x >= 0 && pos.y >= 0 &&
			pos.x < GetRowCount() && pos.y < GetRowCount() &&
			pos.x + pos.y >= size - 1 && pos.x + pos.y <= (size - 1) * 3);
	}
	bool IsMoveLegal(Move move) const
	{
		//std::cout << HexToNotation(move.from) << "-" << HexToNotation(move.to) << " ";
		
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
		GamePositionSet states((2 * size - 1) * 6);
		GamePosition curGameState = GetGamePosition();

		HexPos cur = { -1, size - 1 };
		for (int i = 0; i < 6; i++)
		{
			for (int n = 0; n < size; n++)
			{
				Move move = { cur, cur.GetNeighbor(i + 1) };

				if (IsMoveLegal(move))
				{
					Push(move);
					GamePosition stateAfterMove = GetGamePosition();
					if (!states.Contains(stateAfterMove))
					{
						states.Add(stateAfterMove);
						moves.Append(move);
					}
					RestorePosition(curGameState);
				}

				move.to = cur.GetNeighbor(i + 2);

				if (IsMoveLegal(move))
				{
					Push(move);
					GamePosition stateAfterMove = GetGamePosition();
					if (!states.Contains(stateAfterMove))
					{
						states.Add(stateAfterMove);
						moves.Append(move);
					}
					RestorePosition(curGameState);
				}

				cur = cur.GetNeighbor(i);
			}
		}

		return moves;
	}



	Game& operator=(const Game& other)
	{
		Game tmp(other);

		std::swap(activePlayer, tmp.activePlayer);
		std::swap(playerReserves, tmp.playerReserves);
		std::swap(playerPieces, tmp.playerPieces);
		std::swap(playerMaxPieces, tmp.playerMaxPieces);
		std::swap(maxChain, tmp.maxChain);
		std::swap(size, tmp.size);
		std::swap(tiles, tmp.tiles);

		return *this;
	}
};
