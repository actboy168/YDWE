#pragma once

#include <locale>
#include <string>

namespace ydwe { namespace util { namespace detail {
	typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

	void convert(const char* from, const char* from_end, std::wstring& to, const codecvt_type& cvt);
	void convert(const wchar_t* from, const wchar_t* from_end, std::string& to, const codecvt_type& cvt);

	template<class FromChar, class ToChar> 
	std::basic_string<ToChar> convert(std::basic_string<FromChar> const& from, const codecvt_type& cvt)
	{
		std::basic_string<ToChar> to;
		if (from.size())
		{
			convert(&*from.begin(), &*from.begin() + from.size(), to, cvt);
		}
		return std::move(to);
	}
}}}
