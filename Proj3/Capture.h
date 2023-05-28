#pragma once
#include "HexPos.h"
#include "Vector.h"



struct Capture
{
	HexPos start;
	HexPos end;
	int player = 0;

	Capture() {}
	Capture(HexPos start, HexPos end, int player) : start(start), end(end), player(player) {}
};
