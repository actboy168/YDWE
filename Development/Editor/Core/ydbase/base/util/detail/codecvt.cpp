#include <base/util/detail/codecvt.h>
#include <array>
#include <cassert>
#include <memory>  
#include <stdexcept>
#include <base/util/dynarray.h>

namespace base { namespace detail {
	const std::size_t default_codecvt_buf_size = 256;

	inline void convert_aux(const char* from, const char* from_end, wchar_t* to, wchar_t* to_end, std::wstring& target, const codecvt_type& cvt, conv_method how)
	{
		std::mbstate_t state  = std::mbstate_t();
		const char* from_next;
		wchar_t* to_next;

		if (cvt.in(state, from, from_end, from_next, to, to_end, to_next) == std::codecvt_base::ok)
		{
			target.append(to, to_next);
			return ;
		}
	
		if (how.type() == conv_method::stop)
		{				  
			throw std::logic_error("character conversion failed");
		}

		from_next = from;
		while (from_next != from_end)
		{
			wchar_t to_buf[4];
			int len = cvt.length(state, from_next, from_end, 1);
			if (len <= 0)
				return;

			const char* from_mid;
			if (cvt.in(state, from_next, from_next + len, from_mid, to_buf, to_buf + sizeof(to_buf) / sizeof(to_buf[0]), to_next) == std::codecvt_base::ok)
			{
				assert(from_next + len == from_mid);
				target.append(to_buf, to_next);
				from_next += len;
			}
			else
			{
				if (how.type() == conv_method::replace)
				{
					target.push_back(static_cast<wchar_t>(how.default_char()));
					from_next += len;
				}
				else
				{
					from_next += len;
				}
			}
		}
	}

	inline void convert_aux(const wchar_t* from, const wchar_t* from_end, char* to, char* to_end, std::string& target, const codecvt_type& cvt, conv_method how)
	{
		std::mbstate_t state  = std::mbstate_t();
		const wchar_t* from_next;
		char* to_next;

		if (cvt.out(state, from, from_end, from_next, to, to_end, to_next) == std::codecvt_base::ok)
		{
			target.append(to, to_next);
			return ;
		}
		
		if (how.type() == conv_method::stop)
		{
			throw std::logic_error("character conversion failed");
		}

		from_next = from;
		while (from_next != from_end)
		{
			char to_buf[4];

			const wchar_t* from_mid;
			if (cvt.out(state, from_next, from_next + 1, from_mid, to_buf, to_buf + sizeof(to_buf) / sizeof(to_buf[0]), to_next) == std::codecvt_base::ok)
			{
				assert(from_next + 1 == from_mid);
				target.append(to_buf, to_next);
				from_next += 1;
			}
			else
			{
				if (how.type() == conv_method::replace)
				{
					target.push_back(static_cast<char>(how.default_char()));
					from_next += 1;
				}
				else
				{
					from_next += 1;
				}
			}
		}
	}

	inline void convert(const char* from, const char* from_end, std::wstring& to, const codecvt_type& cvt, conv_method how)
	{
		assert(from);

		if (!from_end) 
		{
			from_end = from + std::strlen(from);
		}

		if (from == from_end) return;

		std::size_t buf_size = (from_end - from) * 3;

		if (buf_size > default_codecvt_buf_size)
		{
			std::dynarray<wchar_t> buf(buf_size);
			convert_aux(from, from_end, buf.begin(), buf.end(), to, cvt, how);
		}
		else
		{
			std::array<wchar_t, default_codecvt_buf_size> buf;
			convert_aux(from, from_end, &*buf.begin(), &*buf.begin() + buf.size(), to, cvt, how);
		}
	}

	inline void convert(const wchar_t* from, const wchar_t* from_end, std::string& to, const codecvt_type& cvt, conv_method how)
	{
		assert(from);

		if (!from_end)
		{
			from_end = from + std::wcslen(from);
		}

		if (from == from_end) return;


		std::size_t buf_size = (from_end - from) * 4;
		buf_size += 4; 

		if (buf_size > default_codecvt_buf_size)
		{
			std::dynarray<char> buf(buf_size);
			convert_aux(from, from_end, buf.begin(), buf.end(), to, cvt, how);
		}
		else
		{
			std::array<char, default_codecvt_buf_size> buf;
			convert_aux(from, from_end, &*buf.begin(), &*buf.begin() + buf.size(), to, cvt, how);
		}
	}
}}
