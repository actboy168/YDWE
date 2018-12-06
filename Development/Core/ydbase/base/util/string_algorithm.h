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
}}
