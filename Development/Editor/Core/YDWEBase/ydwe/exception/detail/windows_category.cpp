#include <ydwe/exception/detail/windows_category.h>
#include <ydwe/exception/detail/error_msg.h>
#include <ydwe/util/unicode.h>
#include <Windows.h>
#include <sstream>

_BASE_BEGIN

namespace exception_detail
{
	const char* windows_category_impl::name() const 
	{ 
		return "windows"; 
	}

	std::string windows_category_impl::message(int error_code) const
	{
		error_msg<wchar_t> buffer;
		unsigned long result = ::FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			error_code,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),  
			reinterpret_cast<LPWSTR>(buffer.get()), 
			0,
			NULL);

		if ((result == 0) || (!buffer))
		{
			std::ostringstream os;
			os << "Unable to get an error message for error code: " << error_code << ".";
			return os.str();
		}

		return std::move(util::w2u(buffer.c_str(), util::conv_method::replace | '?'));
	}

	std::error_condition windows_category_impl::default_error_condition(int error_code) const
	{
		const std::error_condition err_condition = std::system_category().default_error_condition(error_code);
		const std::string err_condition_msg = err_condition.message();
		if (_stricmp(err_condition_msg.c_str(), "unknown error") != 0)
		{
			return err_condition;
		}

		return std::error_condition(error_code, *this);
	}

	const std::error_category& __cdecl windows_category()
	{
		static windows_category_impl instance;
		return instance;
	}

	std::error_code __cdecl create_error_code(unsigned long error_code)
	{
		return std::error_code((int)error_code, windows_category());
	}
}

_BASE_END
