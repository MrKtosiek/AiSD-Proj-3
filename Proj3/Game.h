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

	static const int WHITE = 0;
	static const int BLACK = 1;
	static const char TILE_EMPTY = '_';
	static const char TILE_WHITE = 'W';
	static const char TILE_BLACK = 'B';

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
	
	enum class MoveCode
	{
		OK,
		FIRST_POS_OUT_OF_BOUNDS,
		FIRST_POS_IS_NOT_EDGE,
		SECOND_POS_OUT_OF_BOUNDS,
		SECOND_POS_IS_EDGE,
		NOT_NEIGHBORS,
		ROW_IS_FULL
	};
	enum class CaptureCode
	{
		OK,
		WRONG_INDEX_OF_CHOSEN_ROW,
		WRONG_COLOR_OF_CHOSEN_ROW
	};

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
		: size(size), maxChain(maxChain), gameState(GameState::IN_PROGRESS)
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
		if (playerReserves[WHITE] + playerPieces[WHITE] > playerMaxPieces[WHITE])
		{
			std::cout << "WRONG_WHITE_PAWNS_NUMBER\n";
			return;
		}
		if (playerReserves[BLACK] + playerPieces[BLACK] > playerMaxPieces[BLACK])
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
				pos = pos.RotateRight();
				pos += center;
				tiles[pos.x][pos.y] = board[cursor];
				cursor++;
			}
		}


		Vector<Capture> rows = GetPossibleCaptures();
		if (rows.GetLength() > 0)
		{
			if (rows.GetLength() == 1)
				std::cout << "ERROR_FOUND_" << rows.GetLength() << "_ROW_OF_LENGTH_K\n\n";
			else
				std::cout << "ERROR_FOUND_" << rows.GetLength() << "_ROWS_OF_LENGTH_K\n\n";
			return;
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
				pos = pos.RotateRight();
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
		MoveCode moveCode = IsMoveLegal(move);
		if (moveCode != MoveCode::OK)
		{
			lastBadMove = move;
			gameState = GameState::BAD_MOVE;

			switch (moveCode)
			{
			case MoveCode::FIRST_POS_OUT_OF_BOUNDS:
				std::cout << "BAD_MOVE_" << HexToNotation(move.from) << "_IS_WRONG_INDEX\n\n";
				break;
			case MoveCode::SECOND_POS_OUT_OF_BOUNDS:
				std::cout << "BAD_MOVE_" << HexToNotation(move.to) << "_IS_WRONG_INDEX\n\n";
				break;
			case MoveCode::NOT_NEIGHBORS:
				std::cout << "UNKNOWN_MOVE_DIRECTION\n\n";
				break;
			case MoveCode::FIRST_POS_IS_NOT_EDGE:
				std::cout << "BAD_MOVE_" << HexToNotation(move.from) << "_IS_WRONG_STARTING_FIELD\n\n";
				break;
			case MoveCode::SECOND_POS_IS_EDGE:
				std::cout << "BAD_MOVE_" << HexToNotation(move.to) << "_IS_WRONG_DESTINATION_FIELD\n\n";
				break;
			case MoveCode::ROW_IS_FULL:
				std::cout << "BAD_MOVE_ROW_IS_FULL\n\n";
				break;
			default:
				break;
			}

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


		GamePosition posBeforeMove = GetGamePosition();

		// move the pieces
		Push(move);


		// capture pieces
		for (size_t i = 0; i < move.captures.GetLength(); i++)
		{
			// verify the capture
			CaptureCode captureCode = CheckCapture(move.captures[i]);
			if (captureCode != CaptureCode::OK)
			{
				lastBadMove = move;
				gameState = GameState::BAD_MOVE;

				switch (captureCode)
				{
				case CaptureCode::WRONG_COLOR_OF_CHOSEN_ROW:
					std::cout << "WRONG_COLOR_OF_CHOSEN_ROW\n\n";
					break;
				case CaptureCode::WRONG_INDEX_OF_CHOSEN_ROW:
					std::cout << "WRONG_INDEX_OF_CHOSEN_ROW\n\n";
					break;
				default:
					break;
				}

				RestorePosition(posBeforeMove);
				return;
			}

			CapturePieces(move.captures[i]);
		}


		if (!ResolveCaptures())
		{
			lastBadMove = move;
			gameState = GameState::BAD_MOVE;
			RestorePosition(posBeforeMove);
			return;
		}

		std::cout << "MOVE_COMMITTED\n\n";

		SwitchPlayer();

		lastMove = move;
		gameState = GameState::IN_PROGRESS;
	}

	// automatically perform all captures that don't need any decisions
	// returns true when there were no crossing captures, otherwise returns false and doesn't change the board
	bool ResolveCaptures()
	{
		GamePosition gameStateBefore = GetGamePosition();
		Vector<Capture> possibleCaptures = GetPossibleCaptures();
		for (size_t i = 0; i < possibleCaptures.GetLength(); i++)
		{
			if (CheckCapture(possibleCaptures[i]) == CaptureCode::OK)
			{
				CapturePieces(possibleCaptures[i]);
			}
			else
			{
				RestorePosition(gameStateBefore);
				return false;
			}
		}

		return true;
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
		playerPieces[activePlayer] += 1;
	}
	

	// scans the entire board for chains long enough to capture
	Vector<Capture> GetPossibleCaptures() const
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


				HexPos start = edgePos.GetNeighbor(i + 1);
				HexPos end = start;
				while (IsOnBoard(end))
				{
					size_t rowLength = 0;
					char color = tiles[start.x][start.y];
					if (color != TILE_EMPTY)
					{
						while (IsOnBoard(end) && tiles[end.x][end.y] == color)
						{
							rowLength++;
							end = end.GetNeighbor(i + 1);
						}
						end = end.GetNeighbor(i + 4);

						if (rowLength >= maxChain)
						{
							Capture cap = Capture(start, end, CharToPlayer(color));
							possibleCaptures.Append(cap);
							//std::cout << "Capture: " << HexToNotation(cap.start) << ", " << HexToNotation(cap.end) << ", " << rowLength << ", " << playerColors[cap.player] << "\n";
						}
					}

					end = end.GetNeighbor(i + 1);
					start = end;
				}


				edgePos = edgePos.GetNeighbor(i);
			}
		}

		return possibleCaptures;
	}


	CaptureCode CheckCapture(const Capture& cap) const
	{
		if (tiles[cap.start.x][cap.start.y] != playerColors[cap.player])
			return CaptureCode::WRONG_COLOR_OF_CHOSEN_ROW;

		if (!cap.start.IsOnSameAxis(cap.end))
			return CaptureCode::WRONG_INDEX_OF_CHOSEN_ROW;

		HexPos cur = cap.start;
		HexPos dir = cur.GetDirectionTo(cap.end);
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] == playerColors[cap.player])
		{
			cur += dir;
		}
		if (cur - dir != cap.end)
		{
			return CaptureCode::WRONG_INDEX_OF_CHOSEN_ROW;
		}

		return CaptureCode::OK;
	}

	void CapturePieces(const Capture& cap)
	{
		HexPos cur = cap.start;
		HexPos dir = cur.GetDirectionTo(cap.end);
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			CapturePiece(cur, cap.player);
			cur += dir;
		}
		cur = cap.start - dir;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] != TILE_EMPTY)
		{
			CapturePiece(cur, cap.player);
			cur -= dir;
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

	String CaptureToNotation(const Capture& capture) const
	{
		String result;

		HexPos cur = capture.start;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] == playerColors[capture.player])
		{
			cur -= capture.end;
		}
		cur += capture.end;
		while (IsOnBoard(cur) && tiles[cur.x][cur.y] == playerColors[capture.player])
		{
			result.Append(HexToNotation(cur));
			result.Append(' ');
			cur += capture.end;
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

	String MoveToNotation(const Move& move) const
	{
		String str;

		str.Append(HexToNotation(move.from));
		str.Append('-');
		str.Append(HexToNotation(move.to));
		for (size_t i = 0; i < move.captures.GetLength(); i++)
		{
			str.Append(' ');
			str.Append(playerColors[move.captures[i].player] + ('a' - 'A'));
			str.Append(':');
			str.Append(' ');
			str.Append(HexToNotation(move.captures[i].start));
			str.Append(' ');
			str.Append(HexToNotation(move.captures[i].end));
		}

		return str;
	}

	static int CharToPlayer(char c)
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

	bool IsOnBoard(const HexPos& pos) const
	{
		return (pos.GetDistanceTo(GetCenter()) <= size - 1);
	}
	bool IsEdge(const HexPos& pos) const
	{
		return (pos.GetDistanceTo(GetCenter()) == size);
	}
	bool IsOutOfBounds(const HexPos& pos) const
	{
		return (pos.GetDistanceTo(GetCenter()) > size);
	}


	MoveCode IsMoveLegal(Move move) const
	{
		if (IsOutOfBounds(move.from))
		{
			return MoveCode::FIRST_POS_OUT_OF_BOUNDS;
		}
		if (IsOutOfBounds(move.to))
		{
			return MoveCode::SECOND_POS_OUT_OF_BOUNDS;
		}
		if (!IsEdge(move.from))
		{
			return MoveCode::FIRST_POS_IS_NOT_EDGE;
		}
		if (!IsOnBoard(move.to))
		{
			return MoveCode::SECOND_POS_IS_EDGE;
		}
		if (!move.to.IsNeighbor(move.from))
		{
			return MoveCode::NOT_NEIGHBORS;
		}


		// better coord checks
		//if (IsOnBoard(move.from) || !IsOnBoard(move.to))
		//{
		//	//std::cout << "incorrect coords\n";
		//	return false;
		//}
		//if (!move.to.IsNeighbor(move.from))
		//{
		//	//std::cout << "not neighbors\n";
		//	return false;
		//}

		// check if the pieces can be pushed
		HexPos cur = move.to;
		HexPos dir = move.to - move.from;
		while (IsOnBoard(cur))
		{
			if (tiles[cur.x][cur.y] == TILE_EMPTY)
				return MoveCode::OK;

			cur += dir;
		}

		return MoveCode::ROW_IS_FULL;
	}

	void GenerateCaptures(const Move& move, Vector<Move>& moves, GamePositionSet& states)
	{
		GamePosition pos = GetGamePosition();
		if (ResolveCaptures())
		{
			GamePosition stateAfterMove = GetGamePosition();
			if (!states.Contains(stateAfterMove))
			{
				states.Add(stateAfterMove);
				moves.Append(move);
			}
			RestorePosition(pos);
			return;
		}
		else
		{
			Vector<Capture> captures = GetPossibleCaptures();
			for (size_t i = 0; i < captures.GetLength(); i++)
			{
				CapturePieces(captures[i]);
				Move nextMove = move;
				nextMove.captures.Append(captures[i]);
				GenerateCaptures(nextMove, moves, states);
				RestorePosition(pos);
			}
		}
	}
	void GenerateMove(const Move& move, const GamePosition& gamePos, Vector<Move>& moves, GamePositionSet& states)
	{
		if (IsMoveLegal(move) == MoveCode::OK)
		{
			Push(move);

			GenerateCaptures(move, moves, states);
			RestorePosition(gamePos);
		}

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

				GenerateMove(move, curGameState, moves, states);

				move.to = cur.GetNeighbor(i + 2);

				GenerateMove(move, curGameState, moves, states);

				cur = cur.GetNeighbor(i);
			}
		}

		return moves;
	}



	Game& operator=(const Game& other)
	{
		Game tmp(other);

		std::swap(gameState, tmp.gameState);
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
