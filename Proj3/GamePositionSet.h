#pragma once
#include "String.h"
#include "Vector.h"
#include "GamePosition.h"

class GamePositionSet
{
	struct KeyValuePair
	{
		size_t index;
		KeyValuePair* next;
		KeyValuePair(const size_t index, KeyValuePair* next)
			:index(index), next(next)
		{
		}
	};
private:
	size_t capacity;
	KeyValuePair** chains = nullptr;
	Vector<GamePosition> positions;

	size_t BoardHash(const String& str) const
	{
		size_t hash = 0;
		size_t p = 1;
		for (size_t i = 0; i < str.GetLength(); i++)
		{
			hash += str[i] * p;
			p *= 3; // only 3 possible characters, 'W' 'B' '_'
		}
		return hash % capacity;
	}

public:
	GamePositionSet(size_t capacity)
	{
		this->capacity = capacity;
		chains = new KeyValuePair*[capacity]();
	}
	~GamePositionSet()
	{
		if (chains != nullptr)
		{
			for (size_t i = 0; i < positions.GetLength(); i++)
			{
				Remove(positions[i]);
			}
			delete[] chains;
		}
	}

	void Add(const GamePosition& pos)
	{
		positions.Append(pos);
		size_t hash = BoardHash(pos.board);
		chains[hash] = new KeyValuePair(positions.GetLength() - 1, chains[hash]);
	}

	bool Contains(const GamePosition& pos) const
	{
		size_t hash = BoardHash(pos.board);
		for (KeyValuePair* p = chains[hash]; p != nullptr; p = p->next)
		{
			if (positions[p->index] == pos)
			{
				return true;
			}
		}
		return false;
	}

	void Remove(const GamePosition& pos)
	{
		size_t hash = BoardHash(pos.board);
		KeyValuePair* prev = nullptr;
		for (KeyValuePair* p = chains[hash]; p != nullptr; p = p->next)
		{
			if (positions[p->index] == pos)
			{
				// remove the pair from the chain
				if (p == chains[hash])
					chains[hash] = p->next;
				else
					prev->next = p->next;

				delete p;
				return;
			}
			prev = p;
		}
	}
};
