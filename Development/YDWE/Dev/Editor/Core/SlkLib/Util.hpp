#pragma once

#include "port/config.h"
#include <cassert>
#include <cctype>
#include <deque>
#include <array>

namespace slk
{
	namespace detail
	{
		template <typename _Ty>
		class basic_list_of : public std::deque<_Ty>
		{
		public:
			typedef basic_list_of<_Ty> _Myt;
			typedef std::deque<_Ty>    _Mybase;

			basic_list_of()
				: _Mybase()
			{ }

			template <typename P1>
			basic_list_of(P1 val)
				: _Mybase()
			{
				this->operator () (val);
			}

			template <typename P1, typename P2>
			basic_list_of(P1 val1, P2 val2)
				: _Mybase()
			{
				this->operator () (val1, val2);
			}

			template <typename P1>
			_Myt& operator () ()
			{
				this->push_back(P1());
				return *this;
			}

			template <typename P1>
			_Myt& operator () (P1 val)
			{
				this->push_back(val);
				return *this;
			}

			template <typename P1, typename P2>
			_Myt& operator () (P1 val1, P2 val2)
			{
				return this->operator () (std::make_pair(val1, val2));
			}

			template <class Container>
			operator Container() const
			{
				return Container(this->begin(), this->end());
			}
		};
	}

	template <typename P1>
	detail::basic_list_of<P1> list_of(P1 val)
	{
		return detail::basic_list_of<P1>(val);
	}

	template <typename P1, typename P2>
	detail::basic_list_of<std::pair<P1, P2>> list_of(P1 val1, P2 val2)
	{
		return detail::basic_list_of<std::pair<P1, P2>>(val1, val2);
	}

	class SLKLIB_API noncopyable
	{
	protected:
		noncopyable() { }
		~noncopyable() { }
	private:
		noncopyable(const noncopyable&);
		const noncopyable& operator=(const noncopyable&);
	};

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
			assert(offset >= 0);
			str_buf[--offset] = n % 10 + '0';
			n /= 10;
		} while (n != 0);
		
		return std::move(std::string(str_buf.begin() + offset, str_buf.end()));
	}

	template <class Sequence>
	inline unsigned int Str2UInt(Sequence const& str)
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
