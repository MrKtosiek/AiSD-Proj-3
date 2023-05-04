#pragma once
#include "String.h"

struct HexPos
{
	int x = 0;
	int y = 0;


	HexPos() : x(0), y(0) {}
	HexPos(int x, int y) : x(x), y(y) {}

	// returns neighboring tiles based on index i
	// neighbors are ordered clockwise
	// 
	//     -1 0 1 y
	//
	// -1   4 5
	//  0   3 T 0
	//  1     2 1
	//  x

	HexPos GetNeighbor(size_t i) const
	{
		i = (i % 6 + 6) % 6;
		if (i == 0) return { x    , y + 1 };
		if (i == 1) return { x + 1, y + 1 };
		if (i == 2) return { x + 1, y     };
		if (i == 3) return { x    , y - 1 };
		if (i == 4) return { x - 1, y - 1 };
		if (i == 5) return { x - 1, y     };
		return { INT_MAX, INT_MAX };
	}

	bool IsNeighbor(HexPos other) const
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
	HexPos operator-() const
	{
		return { -x,-y };
	}

	bool operator==(const HexPos& other) const
	{
		return x == other.x && y == other.y;
	}
};
