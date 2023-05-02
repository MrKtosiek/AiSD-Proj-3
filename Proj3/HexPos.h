#pragma once
#include "String.h"

struct HexPos
{
	int x = 0;
	int y = 0;


	HexPos() : x(0), y(0) {}
	HexPos(int x, int y) : x(x), y(y) {}

	HexPos GetNeighbor(size_t i) // i should always be in the range 0-5
	{
		if (i == 0) return { x - 1, y - 1 };
		if (i == 1) return { x - 1, y     };
		if (i == 2) return { x    , y + 1 };
		if (i == 3) return { x + 1, y + 1 };
		if (i == 4) return { x + 1, y     };
		if (i == 5) return { x    , y - 1 };
		return { INT_MAX, INT_MAX };
	}

	bool IsNeighbor(HexPos other)
	{
		for (size_t i = 0; i < 6; i++)
		{
			if (GetNeighbor(i) == other)
				return true;
		}
		return false;
	}

	HexPos& operator+=(const HexPos& other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
	friend HexPos operator+(HexPos l, const HexPos& r)
	{
		l += r;
		return l;
	}
	HexPos& operator-=(const HexPos& other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}
	friend HexPos operator-(HexPos l, const HexPos& r)
	{
		l -= r;
		return l;
	}
	HexPos operator-()
	{
		return { -x,-y };
	}

	bool operator==(const HexPos& other) const
	{
		return x == other.x && y == other.y;
	}
};
