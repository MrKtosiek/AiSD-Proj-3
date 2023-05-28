#pragma once
#include "HexPos.h"
#include "Vector.h"
#include "Capture.h"

struct Move
{
	HexPos from;
	HexPos to;

	Vector<Capture> captures;
};
