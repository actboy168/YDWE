#pragma once

#include <slk/port/config.h>
#include <base/util/string_view.h>
#include <ctype.h>

namespace slk
{
	namespace detail
	{
		inline std::string_view::const_iterator find_begin(std::string_view::const_iterator InBegin, std::string_view::const_iterator InEnd)
		{
			for (std::string_view::const_iterator It = InBegin; It != InEnd; ++It)
			{
				if (!isspace((unsigned char)*It))
					return It;
			}

			return InEnd;
		}

		inline std::string_view::const_iterator find_end(std::string_view::const_iterator InBegin, std::string_view::const_iterator InEnd)
		{
			for (std::string_view::const_iterator It = InEnd; It != InBegin;)
			{
				if (!isspace((unsigned char)*(--It)))
				{
					return ++It;
				}
			}

			return InBegin;
		}
	}

	inline void trim_left(std::string_view& Input)
	{
		std::string_view::const_iterator TrimBegin = detail::find_begin(Input.begin(), Input.end());
		Input.remove_prefix(std::distance(Input.begin(), TrimBegin));
	}

	inline void trim_right(std::string_view& Input)
	{
		std::string_view::const_iterator TrimEnd = detail::find_end(Input.begin(), Input.end());
		Input.remove_suffix(std::distance(TrimEnd, Input.end()));
	}

	inline void trim(std::string_view& Input)
	{
		trim_right(Input);
		trim_left(Input);
	}

	template <typename Iterator>
	std::string_view make_string_view(Iterator first, Iterator last)
	{
		if (first < last)
		{
			return std::string_view(first, std::distance(first, last));
		}
		else
		{
			return std::string_view();
		}
	}
	inline std::string_view trim_left_copy(std::string_view& Input)
	{
		std::string_view::const_iterator TrimEnd = Input.end();
		std::string_view::const_iterator TrimBegin = detail::find_begin(Input.begin(), Input.end());
		return std::move(make_string_view(TrimBegin, TrimEnd));
	}
	inline std::string_view trim_right_copy(std::string_view& Input)
	{
		std::string_view::const_iterator TrimEnd = detail::find_end(Input.begin(), Input.end());
		std::string_view::const_iterator TrimBegin = Input.begin();
		return std::move(make_string_view(TrimBegin, TrimEnd));
	}
	inline std::string_view trim_copy(std::string_view::const_iterator ItBegin, std::string_view::const_iterator ItEnd)
	{
		std::string_view::const_iterator TrimEnd = detail::find_end(ItBegin, ItEnd);
		std::string_view::const_iterator TrimBegin = detail::find_begin(ItBegin, TrimEnd);
		return std::move(make_string_view(TrimBegin, TrimEnd));
	}

	template <typename CharT>
	struct basic_char_equal
	{
		CharT c_;

		basic_char_equal(CharT Ch)
			: c_(Ch)
		{ }

		bool operator()(CharT Ch) const
		{
			return c_ == Ch;
		}
	};
	typedef basic_char_equal<char>    char_equal;
	typedef basic_char_equal<wchar_t> wchar_equal;

	template <typename ResultT, typename SequenceT, typename PredicateT>
	inline ResultT& split(ResultT& Result, SequenceT const& Input, PredicateT IsFound)
	{
		typedef typename SequenceT::const_iterator ForwardIteratorT;
		ForwardIteratorT InBegin = Input.begin();
		ForwardIteratorT InEnd = Input.end();
		ForwardIteratorT InCur = Input.begin();

		for (ForwardIteratorT It = InBegin; It != InEnd; )
		{
			for (; It != InEnd; ++It)
			{
				if (IsFound(*It))
				{
					Result.push_back(ResultT::value_type(InCur, It));
					break;
				}
			}

			if (It != InEnd)
			{
				InCur = ++It;
			}
		}
		if (InCur != InEnd)
		{
			Result.push_back(ResultT::value_type(InCur, InEnd));
		}

		return Result;
	}

	template <typename InputT, typename CharT>
	inline typename InputT::value_type concat(InputT const& Input, CharT C)
	{
		typename InputT::value_type Result;
		
		typedef typename InputT::const_iterator IteratorT;
		IteratorT InBegin = Input.begin();
		IteratorT InEnd   = Input.end();
		IteratorT InCur   = Input.begin();

		if (InBegin != InEnd)
		{
			Result.append(*InBegin);
			for (IteratorT It = ++InBegin; It != InEnd; ++It)
			{
				Result.push_back(C);
				Result.append(*It);
			}
		}

		return std::move(Result);
	}
}
