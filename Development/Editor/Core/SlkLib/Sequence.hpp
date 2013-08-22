#pragma once

#include <cctype>
#include <locale>
#include "Util.hpp"

namespace slk
{
	namespace detail
	{
		template <typename ForwardIteratorT, typename PredicateT>
		ForwardIteratorT find_begin(ForwardIteratorT InBegin, ForwardIteratorT InEnd, PredicateT IsFound)
		{
			for (ForwardIteratorT It = InBegin; It != InEnd; ++It)
			{
				if (!IsFound(*It))
					return It;
			}

			return InEnd;
		}

		template <typename ForwardIteratorT, typename PredicateT>
		ForwardIteratorT find_end(ForwardIteratorT InBegin, ForwardIteratorT InEnd, PredicateT IsFound)
		{
			for (ForwardIteratorT It = InEnd; It != InBegin; )
			{
				if (!IsFound(*(--It)))
				{
					return ++It;
				}
			}

			return InBegin;
		}

		template<class CharT>
		struct ctype_facet_ptr
		{
			std::locale locale_;
			const std::ctype<CharT>* facet_ptr_;

			ctype_facet_ptr()
				: locale_()
				, facet_ptr_(&std::use_facet<std::ctype<CharT>>(locale_))
			{ }

			const std::ctype<CharT>* operator->() const
			{
				return facet_ptr_;
			}
		};

	}

	namespace ctype
	{
		struct is_space
		{
			template<class CharT>
			bool operator()(CharT Ch) const
			{
				return Singleton<detail::ctype_facet_ptr<CharT>>::instance()->is(std::ctype_base::space, Ch);
			}
		};
	}

	template<typename CharT>
	bool is_space(CharT Ch)
	{
		return ctype::is_space()(Ch);
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

	template <typename SequenceT, typename PredicateT>
	inline void trim_left(SequenceT& Input, PredicateT IsFound)
	{
		typename SequenceT::iterator TrimBegin = detail::find_begin(Input.begin(), Input.end(), IsFound);
		Input.erase(Input.begin(), TrimBegin);
	}

	template <typename SequenceT, typename PredicateT>
	inline void trim_right(SequenceT& Input, PredicateT IsFound)
	{
		typename SequenceT::iterator TrimEnd = detail::find_end(Input.begin(), Input.end(), IsFound);
		Input.erase(TrimEnd, Input.end());
	}

	template <typename SequenceT, typename PredicateT>
	inline void trim(SequenceT& Input, PredicateT IsFound)
	{
		trim_right(Input, IsFound);
		trim_left(Input, IsFound);
	}

	template <typename SequenceT, typename PredicateT>
	inline SequenceT trim_left_copy(SequenceT& Input, PredicateT IsFound)
	{
		typename SequenceT::const_iterator TrimEnd = Input.end();
		typename SequenceT::const_iterator TrimBegin = detail::find_begin(Input.begin(), Input.end(), IsFound);
		return std::move(SequenceT(TrimBegin, TrimEnd));
	}

	template <typename SequenceT, typename PredicateT>
	inline SequenceT trim_right_copy(SequenceT& Input, PredicateT IsFound)
	{
		typename SequenceT::const_iterator TrimEnd = detail::find_end(Input.begin(), Input.end(), IsFound);
		typename SequenceT::const_iterator TrimBegin = Input.begin();
		return std::move(SequenceT(TrimBegin, TrimEnd));
	}

	template <typename SequenceT, typename PredicateT>
	inline SequenceT trim_copy(typename SequenceT::const_iterator ItBegin, typename SequenceT::const_iterator ItEnd, PredicateT IsFound)
	{
		typename SequenceT::const_iterator TrimEnd = detail::find_end(ItBegin, ItEnd, IsFound);
		typename SequenceT::const_iterator TrimBegin = detail::find_begin(ItBegin, TrimEnd, IsFound);
		return std::move(SequenceT(TrimBegin, TrimEnd));
	}

	template <typename SequenceT, typename PredicateT>
	inline SequenceT trim_copy(const SequenceT& Input, PredicateT IsFound)
	{
		return std::move(trim_copy<SequenceT, PredicateT>(Input.cbegin(), Input.cend(), IsFound));
	}


	template <typename SequenceT, typename PredicateT>
	typename SequenceT::iterator find_begin(SequenceT& Input, PredicateT IsFound)
	{
		typename SequenceT::iterator InBegin = Input.begin();
		typename SequenceT::iterator InEnd = Input.end();
		for (typename SequenceT::iterator It = InBegin; It != InEnd; ++It)
		{
			if (IsFound(*It))
				return It;
		}

		return InEnd;
	}

	template <typename SequenceT, typename PredicateT>
	typename SequenceT::iterator find_end(SequenceT& Input, PredicateT IsFound)
	{
		typename SequenceT::iterator InBegin = Input.begin();
		typename SequenceT::iterator InEnd = Input.end();
		for (typename SequenceT::iterator It = InEnd; It != InBegin; )
		{
			if (IsFound(*(--It)))
			{
				return ++It;
			}
		}

		return InEnd;
	}

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
					Result.push_back(SequenceT(InCur, It));
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
			Result.push_back(SequenceT(InCur, InEnd));
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
