#pragma once

#include <slk/port/config.h>
#include <deque>

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
}
