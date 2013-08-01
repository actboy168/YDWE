#include <ydwe/util/detail/ansi_codecvt_facet.h>
#include <cwchar>
#include <windows.h>

_BASE_BEGIN 
namespace util { namespace detail {

std::codecvt_base::result ansi_codecvt_facet::do_in(std::mbstate_t &, const char* from, const char* from_end, const char*& from_next, wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const
{
	int count;
	if ((count = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, from, from_end - from, to, to_end - to)) == 0) 
	{
		return error;
	}

	from_next = from_end;
	to_next = to + count;
	*to_next = L'\0';
	return ok;
}

std::codecvt_base::result ansi_codecvt_facet::do_out(std::mbstate_t &, const wchar_t* from, const wchar_t* from_end, const wchar_t*  & from_next, char* to, char* to_end, char* & to_next) const
{
	int count;
	if ((count = ::WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, from, from_end - from, to, to_end - to, 0, 0)) == 0)
	{
		return error;
	}

	from_next = from_end;
	to_next = to + count;
	*to_next = '\0';
	return ok;
}

}}
_BASE_END
