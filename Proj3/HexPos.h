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
	// -1     4 5
	//  0   3 T 0
	//  1   2 1
	//  x
	HexPos GetNeighbor(size_t i) const
	{
		i = (i % 6 + 6) % 6;

		if (i == 0) return { x    , y + 1 };
		if (i == 1) return { x + 1, y     };
		if (i == 2) return { x + 1, y - 1 };
		if (i == 3) return { x    , y - 1 };
		if (i == 4) return { x - 1, y     };
		if (i == 5) return { x - 1, y + 1 };

		return { INT_MAX, INT_MAX }; // this should never be reached
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

	size_t GetNeighborIndex(HexPos other) const
	{
		for (size_t i = 0; i < 6; i++)
		{
			if (GetNeighbor(i) == other)
				return i;
		}
		return 0;
	}



	HexPos RotateLeft() const
	{
		int q = -x - y;
		int r = x;
		//int s = y;
		return { -q, -r };
	}
	HexPos RotateRight() const
	{
		int q = -x - y;
		//int r = x;
		int s = y;
		return { -s, -q };
	}

	size_t GetDistanceTo(const HexPos& dest) const
	{
		int q1 = -x - y;
		int r1 = x;
		int s1 = y;
		int q2 = -dest.x - dest.y;
		int r2 = dest.x;
		int s2 = dest.y;

		return ((abs(q1 - q2) + abs(r1 - r2) + abs(s1 - s2)) / 2);
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
