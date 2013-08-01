#include <ydwe/util/detail/codecvt.h>
#include <cassert>
#include <memory>
#include <ydwe/exception/windows_exception.h>

_BASE_BEGIN namespace util { namespace detail {
	const std::size_t default_codecvt_buf_size = 256;

	void convert_aux(const char* from, const char* from_end, wchar_t* to, wchar_t* to_end, std::wstring& target, const codecvt_type& cvt)
	{
		std::mbstate_t state  = std::mbstate_t();
		const char* from_next;
		wchar_t* to_next;

		std::codecvt_base::result res;

		if ((res = cvt.in(state, from, from_end, from_next, to, to_end, to_next)) != std::codecvt_base::ok)
		{
			throw windows_exception(L"character conversion failed");
		}
		target.append(to, to_next); 
	}

	void convert_aux(const wchar_t* from, const wchar_t* from_end, char* to, char* to_end, std::string& target, const codecvt_type& cvt)
	{
		std::mbstate_t state  = std::mbstate_t();
		const wchar_t* from_next;
		char* to_next;

		std::codecvt_base::result res;

		if ((res = cvt.out(state, from, from_end, from_next, to, to_end, to_next)) != std::codecvt_base::ok)
		{
			throw windows_exception(L"character conversion failed");
		}
		target.append(to, to_next); 
	}

	void convert(const char* from, const char* from_end, std::wstring& to, const codecvt_type& cvt)
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
			std::unique_ptr<wchar_t[]> buf(new wchar_t[buf_size]);
			convert_aux(from, from_end, buf.get(), buf.get()+buf_size, to, cvt);
		}
		else
		{
			wchar_t buf[default_codecvt_buf_size];
			convert_aux(from, from_end, buf, buf+default_codecvt_buf_size, to, cvt);
		}
	}

	void convert(const wchar_t* from, const wchar_t* from_end, std::string& to, const codecvt_type& cvt)
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
			std::unique_ptr<char[]> buf(new char[buf_size]);
			convert_aux(from, from_end, buf.get(), buf.get()+buf_size, to, cvt);
		}
		else
		{
			char buf[default_codecvt_buf_size];
			convert_aux(from, from_end, buf, buf+default_codecvt_buf_size, to, cvt);
		}
	}
}}}
