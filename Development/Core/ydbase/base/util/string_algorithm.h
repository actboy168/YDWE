#pragma once

#include <base/util/string_algorithm.h>

namespace base { namespace  algorithm {
	namespace detail
	{
		template <typename ItorT>
		inline ItorT find_begin(ItorT begin, ItorT end)
		{
			for (ItorT it = begin; it != end; ++it)
			{
				if (!isspace((unsigned char)*it))
					return it;
			}
			return end;
		}

		template <typename ItorT>
		inline ItorT find_end(ItorT begin, ItorT end)
		{
			for (ItorT it = end; it != begin;)
			{
				if (!isspace((unsigned char)*(--it)))
				{
					return ++it;
				}
			}
			return begin;
		}
	}

	template <typename StrT>
	inline void trim(StrT& str)
	{
		str.remove_prefix(std::distance(str.begin(), detail::find_begin(str.begin(), str.end())));
		str.remove_suffix(std::distance(detail::find_end(str.begin(), str.end()), str.end()));
	}	      
	
	template <typename ResultT, typename StrT>
	inline void split(ResultT& Result, StrT& input, typename StrT::value_type c)
	{
		for (size_t pos = 0;;)
		{
			size_t next = input.find(c, pos);
			if (next != StrT::npos)
			{
				Result.push_back(input.substr(pos, next - pos));
				pos = next + 1;
			}
			else
			{
				Result.push_back(input.substr(pos));
				break;
			}
		}
	}
}}
