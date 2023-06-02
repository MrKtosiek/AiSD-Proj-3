#pragma once

enum class GameState
{
	IN_PROGRESS,
	BAD_MOVE,
	WHITE_WIN,
	BLACK_WIN,
	DEAD_LOCK,
	INCORRECT_BOARD
};
