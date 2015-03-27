#pragma once

#include <base/util/detail/utf8_codecvt.h>

namespace base { namespace detail {
	namespace
	{
		inline bool invalid_continuing_octet(unsigned char octet_1) 
		{ 
			return (octet_1 < 0x80|| 0xbf< octet_1); 
		}

		inline bool invalid_leading_octet(unsigned char octet_1) 
		{ 
			return (0x7f < octet_1 && octet_1 < 0xc0) || (octet_1 > 0xfd);
		}

		inline unsigned int get_octet_count(unsigned char   lead_octet)
		{
			if (lead_octet <= 0x7f) 
				return 1;

			if (0xc0 <= lead_octet && lead_octet <= 0xdf) 
				return 2;
			else if (0xe0 <= lead_octet && lead_octet <= 0xef) 
				return 3;
			else if (0xf0 <= lead_octet && lead_octet <= 0xf7) 
				return 4;
			else if (0xf8 <= lead_octet && lead_octet <= 0xfb) 
				return 5;
			else 
				return 6;
		}

		inline unsigned int get_cont_octet_count(unsigned char lead_octet) 
		{ 
			return get_octet_count(lead_octet) - 1; 
		}

		template<std::size_t s>
		int get_cont_octet_out_count_impl(wchar_t word)
		{
			if (word < 0x80) 
			{
				return 0;
			}
			if (word < 0x800)
			{
				return 1;
			}
			return 2;
		}

		template<>
		int get_cont_octet_out_count_impl<4>(wchar_t word)
		{
			if (word < 0x80) 
			{
				return 0;
			}

			if (word < 0x800)
			{
				return 1;
			}

#if !defined(WCHAR_MAX)
#   error WCHAR_MAX not defined!
#endif

#if defined(_MSC_VER) && _MSC_VER <= 1310
			return 2;
#elif WCHAR_MAX > 0x10000
			if (word < 0x10000) 
			{
				return 2;
			}
			if (word < 0x200000) 
			{
				return 3;
			}
			if (word < 0x4000000) 
			{
				return 4;
			}
			return 5;

#else
			return 2;
#endif
		}

		inline int get_cont_octet_out_count(wchar_t word) 
		{
			return get_cont_octet_out_count_impl<sizeof(wchar_t)>(word);
		}
	}

	inline std::codecvt_base::result utf8_codecvt_facet::do_in(std::mbstate_t& /*state*/, const char* from, const char* from_end, const char*& from_next, wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const
	{
		while (from != from_end && to != to_end)
		{
			if (invalid_leading_octet(*from))
			{
				from_next = from;
				to_next = to;
				return std::codecvt_base::error;
			}
		
			const int cont_octet_count = get_cont_octet_count(*from);
			const wchar_t octet1_modifier_table[] = {
				0x00, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc
			};
		
			wchar_t ucs_result = (unsigned char)(*from++) - octet1_modifier_table[cont_octet_count];
		
			int i = 0;
			while (i != cont_octet_count && from != from_end) 
			{
				if (invalid_continuing_octet(*from))
				{
					from_next   = from;
					to_next =   to;
					return std::codecvt_base::error;
				}
		
				ucs_result *= (1 << 6); 
		
				ucs_result += (unsigned char)(*from++) - 0x80;
				++i;
			}
		
			if (from == from_end && i != cont_octet_count) 
			{
				from_next = from - (i+1); 
				to_next = to;
				return std::codecvt_base::partial;
			}
			*to++ = ucs_result;
		}
		from_next = from;
		to_next = to;
		
		if (from == from_end) 
			return std::codecvt_base::ok;
		else 
			return std::codecvt_base::partial;
	}
	
	inline std::codecvt_base::result utf8_codecvt_facet::do_out(std::mbstate_t&, const wchar_t* from, const wchar_t* from_end, const wchar_t*& from_next, char* to, char* to_end, char*& to_next) const
	{
		const wchar_t octet1_modifier_table[] = {
			0x00, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc
		};
	
		wchar_t max_wchar = (std::numeric_limits<wchar_t>::max)();
		while (from != from_end && to != to_end)
		{
			if (*from  > max_wchar)
			{
				from_next = from;
				to_next = to;
				return std::codecvt_base::error;
			}
	
			int cont_octet_count = get_cont_octet_out_count(*from);
	
			int shift_exponent = (cont_octet_count) * 6;
	
			*to++ = static_cast<char>(octet1_modifier_table[cont_octet_count] + (unsigned char)(*from / (1 << shift_exponent)));
	
			int i   = 0;
			while (i != cont_octet_count && to != to_end) 
			{
				shift_exponent -= 6;
				*to++ = static_cast<char>(0x80 + ((*from / (1 << shift_exponent)) % (1 << 6)));
				++i;
			}

			if (to == to_end && i != cont_octet_count)
			{
				from_next = from;
				to_next = to - (i+1);
				return std::codecvt_base::partial;
			}
			++from;
		}
		from_next = from;
		to_next = to;
		if (from == from_end) 
			return std::codecvt_base::ok;
		else 
			return std::codecvt_base::partial;
	}

	inline std::codecvt_base::result utf8_codecvt_facet::do_unshift(std::mbstate_t&, char * from, char * /*to*/, char * & next) const 
	{
		next = from;
		return ok;
	}

	inline int utf8_codecvt_facet::do_length(const std::mbstate_t &, const char* from, const char* from_end, std::size_t max_limit) const throw()
	{
		int last_octet_count = 0;
		std::size_t char_count = 0;
		const char* from_next = from;

		while (from_next+last_octet_count <= from_end && char_count <= max_limit) 
		{
			from_next += last_octet_count;
			last_octet_count = get_octet_count(*from_next);
			++char_count;
		}
		return static_cast<int>(from_next-from);
	}
	
}}
