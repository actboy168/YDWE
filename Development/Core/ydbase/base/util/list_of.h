#pragma once

#include <deque>

namespace base
{
	namespace detail
	{
		template <typename T>
		class basic_list_of
			: public std::deque<T>
		{
		public:
			typedef basic_list_of<T> class_type;
			typedef std::deque<T>    base_type;

			basic_list_of()
				: base_type()
			{ }

			template <typename P1>
			basic_list_of(P1 val)
				: base_type()
			{
				this->operator () (val);
			}

			template <typename P1, typename P2>
			basic_list_of(P1 val1, P2 val2)
				: base_type()
			{
				this->operator () (val1, val2);
			}

			template <typename P1>
			class_type& operator () ()
			{
				this->push_back(P1());
				return *this;
			}

			template <typename P1>
			class_type& operator () (P1 val)
			{
				this->push_back(val);
				return *this;
			}

			template <typename P1, typename P2>
			class_type& operator () (P1 val1, P2 val2)
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
