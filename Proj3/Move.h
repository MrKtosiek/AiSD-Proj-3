#pragma once
#include "HexPos.h"
#include "Capture.h"
#include <vector>

struct Move
{
	HexPos from;
	HexPos to;

	std::vector<Capture> captures;
};
