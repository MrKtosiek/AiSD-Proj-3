#pragma once
#include "HexPos.h"
#include "Vector.h"



struct Capture
{
	HexPos pos;
	HexPos dir;
	int player = 0;

	Capture() {}
	Capture(HexPos pos, HexPos dir, int player) : pos(pos), dir(dir), player(player) {}
};
