#pragma once

#include <slk/port/config.h>
#include <array>
#include <string>
#include <cassert>

namespace slk
{
	template <size_t n, size_t m>
	struct integral_log
	{
		enum { value = integral_log<n/m, m>::value + 1 };
	};

	template <size_t m>
	struct integral_log<0, m> 
	{
		enum { value = 0 };
	};

	template <size_t n>
	struct integral_log10 :public integral_log<n, 10> 
	{ };

	inline std::string Int2Str(size_t n)
	{
		std::array<char, integral_log10<size_t(-1)>::value> str_buf;
		size_t offset = integral_log10<size_t(-1)>::value;

		do
		{
			assert(offset > 0);
			str_buf[--offset] = n % 10 + '0';
			n /= 10;
		} while (n != 0);

		return std::move(std::string(str_buf.begin() + offset, str_buf.end()));
	}

	template <class SequenceT>
	inline unsigned int Str2UInt(SequenceT const& str)
	{
		unsigned int sum = 0;
		foreach(char c, str)
		{
			if (!isdigit(static_cast<uint8_t>(c)))
			{
				break;
			}

			sum = sum * 10 + (c-'0');
		}

		return sum;
	}

	inline int Str2Int(std::string const& str)
	{
		if (str.size() > 0 && str[0] == '-')
		{
			return -static_cast<int>(Str2UInt(str.substr(1)));
		}
		else
		{
			return static_cast<int>(Str2UInt(str));
		}
	}

	inline float Str2UFloat(std::string const& str)
	{
		bool dot = false;
		float y = 0.1f;
		float sum = 0.f;
		foreach(char c, str)
		{
			if (dot)
			{
				if (!isdigit(static_cast<uint8_t>(c)))
				{
					break;
				}

				sum = sum + (c-'0')*y;
				y /= 10.f;
			}
			else
			{
				if (c == '.') { dot = true; continue; }

				if (!isdigit(static_cast<uint8_t>(c)))
				{
					break;
				}

				sum = sum * 10.f + (c-'0');
			}
		}

		return sum;
	}

	inline float Str2Float(std::string const& str)
	{
		if (str.size() > 0 && str[0] == '-')
		{
			return -Str2UFloat(str.substr(1));
		}
		else
		{
			return Str2UFloat(str);
		}
	}
}
