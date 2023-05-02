#pragma once
#include <iostream>

class String
{
private:
	size_t length = 0;
	size_t capacity = 4;
	char* chars = nullptr;

public:

	String()
	{
		chars = new char[capacity]();
	}
	String(const String& orig)
	{
		length = orig.length;
		capacity = orig.capacity;
		chars = new char[orig.capacity]();
		for (size_t i = 0; i < capacity; i++)
		{
			chars[i] = orig.chars[i];
		}
	}
	String(const char* str)
	{
		capacity = strlen(str) + 1;
		chars = new char[capacity]();
		for (int i = 0; str[i] != 0; i++)
		{
			Append(str[i]);
		}
	}
	~String()
	{
		if (chars != nullptr)
			delete[] chars;
	}

	size_t GetLength() const
	{
		return length;
	}

	void Append(const char& c)
	{
		length++;

		if (length >= capacity)
		{
			capacity *= 2;
			char* tmp = new char[capacity]();

			for (size_t i = 0; i < length; i++)
			{
				tmp[i] = chars[i];
			}

			delete[] chars;
			chars = tmp;
		}

		chars[length - 1] = c;
	}

	int ToInt() const
	{
		int value = 0;
		for (size_t i = 0; i < length; i++)
		{
			value *= 10;
			char digit = chars[i];
			if (digit >= '0' && digit <= '9')
			{
				value += digit - '0';
			}
		}
		return value;
	}
	void Capitalize()
	{
		for (size_t i = 0; i < length; i++)
		{
			if (chars[i] >= 'a' && chars[i] <= 'z')
				chars[i] += 'A' - 'a';
		}
	}

	char& operator[](size_t index) const
	{
		return chars[index];
	}
	bool operator==(const String& other) const
	{
		if (length != other.length)
			return false;

		for (size_t i = 0; i < length; i++)
		{
			if (chars[i] != other.chars[i])
			{
				return false;
			}
		}

		return true;
	}
	String& operator=(const int orig)
	{
		int value = orig;
		do
		{
			Append((value % 10) + '0');
			value /= 10;

		} while (value != 0);

		// flip the string
		for (size_t i = 0; i < length / 2; i++)
		{
			char tmp = chars[i];
			chars[i] = chars[length - i - 1];
			chars[length - i - 1] = tmp;
		}

		return *this;
	}
	String& operator=(const String& orig)
	{
		String tmp(orig);
		length = tmp.length;
		capacity = tmp.capacity;
		char* t = tmp.chars;
		tmp.chars = chars;
		chars = t;
		return *this;
	}
	friend std::ostream& operator<<(std::ostream& ostr, const String& str)
	{
		ostr << str.chars;
		return ostr;
	}
	friend std::istream& operator>>(std::istream& istr, String& str)
	{
		memset(str.chars, 0, str.capacity);
		str.length = 0;
		char c;
		istr >> std::ws;
		while (!isspace(istr.peek()))
		{
			istr >> c;
			str.Append(c);
		}
		return istr;
	}
};
