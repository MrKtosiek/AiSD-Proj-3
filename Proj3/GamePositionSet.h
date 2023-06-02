#pragma once
#include "GamePosition.h"
#include <vector>

class GamePositionSet
{
	struct KeyIndexPair
	{
		size_t index;
		KeyIndexPair* next;
		KeyIndexPair(const size_t index, KeyIndexPair* next)
			:index(index), next(next)
		{
		}
	};
private:
	size_t capacity;
	KeyIndexPair** chains = nullptr;
	std::vector<GamePosition> positions;

	size_t BoardHash(const std::string& str) const
	{
		size_t hash = 0;
		size_t p = 1;
		for (size_t i = 0; i < str.length(); i++)
		{
			hash += str[i] * p;
			p *= 3; // only 3 possible characters, 'W' 'B' '_'
		}
		return hash % capacity;
	}

public:
	explicit GamePositionSet(size_t capacity)
	{
		this->capacity = capacity;
		chains = new KeyIndexPair*[capacity]();
	}
	GamePositionSet(const GamePositionSet& orig)
		: capacity(orig.capacity)
	{
		chains = new KeyIndexPair*[capacity]();

		for (size_t i = 0; i < capacity; i++)
		{
			for (KeyIndexPair* p = orig.chains[i]; p != nullptr; p = p->next)
			{
				Add(orig.positions[p->index]);
			}
		}
	}
	~GamePositionSet()
	{
		if (chains != nullptr)
		{
			for (size_t i = 0; i < positions.size(); i++)
			{
				Remove(positions[i]);
			}
			delete[] chains;
		}
	}

	void Add(const GamePosition& pos)
	{
		positions.push_back(pos);
		size_t hash = BoardHash(pos.board);
		chains[hash] = new KeyIndexPair(positions.size() - 1, chains[hash]);
	}

	bool Contains(const GamePosition& pos) const
	{
		size_t hash = BoardHash(pos.board);
		for (KeyIndexPair* p = chains[hash]; p != nullptr; p = p->next)
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
		KeyIndexPair* prev = nullptr;
		for (KeyIndexPair* p = chains[hash]; p != nullptr; p = p->next)
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

	GamePositionSet& operator=(const GamePositionSet& other)
	{
		GamePositionSet tmp(other);

		std::swap(capacity, tmp.capacity);
		std::swap(chains, tmp.chains);
		std::swap(positions, tmp.positions);

		return *this;
	}
};
