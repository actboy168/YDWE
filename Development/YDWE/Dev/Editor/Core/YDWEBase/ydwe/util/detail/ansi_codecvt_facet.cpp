#include <ydwe/util/detail/ansi_codecvt_facet.h>
#include <cwchar>
#include <windows.h>

_BASE_BEGIN
namespace util { namespace detail {

std::codecvt_base::result ansi_codecvt_facet::do_in(std::mbstate_t &, const char* from, const char* from_end, const char*& from_next, wchar_t* to, wchar_t* to_end, wchar_t*& to_next) const
{
	int count;
	if ((count = ::MultiByteToWideChar(CP_ACP, 0, from, from_end - from, to, to_end - to)) == 0) 
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
	if ((count = ::WideCharToMultiByte(CP_ACP, 0, from, from_end - from, to, to_end - to, 0, 0)) == 0)
	{
		return error;
	}

	from_next = from_end;
	to_next = to + count;
	*to_next = '\0';
	return ok;
}


std::codecvt_base::result ansi_codecvt_facet::do_unshift(std::mbstate_t&, char* to, char* /*to_end*/, char* & next) const  
{ 
	next = to;
	return ok;
} 

int ansi_codecvt_facet::do_length(std::mbstate_t&, const char* from, const char* from_end, std::size_t max_limit) const
{ 
	int count = ::MultiByteToWideChar(CP_ACP, 0, from, from_end - from, NULL, 0);
	if (count <= 0)
	{
		return 0;
	}
	return (static_cast<std::size_t>(count) > max_limit)? static_cast<int>(max_limit): count; 
}

int ansi_codecvt_facet::do_max_length() const throw ()
{
	return 2;
}

}}
_BASE_END
