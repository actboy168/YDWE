#include <base/exception/windows_exception.h>
#include <base/util/unicode.h>
#include <Windows.h>
#include <string_view>
#include <sstream>	
#include <system_error>

namespace base {

	class windows_category : public std::error_category {
	public:
		virtual const char* name() const noexcept {
			return "windows";
		}
		virtual std::string message(int error_code) const {
			exception::werrormsg buffer;
			unsigned long result = ::FormatMessageW(
				FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				error_code,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				reinterpret_cast<LPWSTR>(buffer.get()),
				0,
				NULL);

			if ((result == 0) || (!buffer)) {
				std::ostringstream os;
				os << "Unable to get an error message for error code: " << error_code << ".";
				return os.str();
			}
			std::wstring_view str(buffer.c_str());
			while (str.size() && ((str.back() == L'\n') || (str.back() == L'\r'))) {
				str.remove_suffix(1);
			}
			return std::move(w2u(str, conv_method::replace | '?'));
		}
		virtual std::error_condition default_error_condition(int error_code) const noexcept {
			const std::error_condition err_condition = std::system_category().default_error_condition(error_code);
			const std::string err_condition_msg = err_condition.message();
			if (_stricmp(err_condition_msg.c_str(), "unknown error") != 0) {
				return err_condition;
			}
			return std::error_condition(error_code, *this);
		}
	};

	windows_exception::windows_exception(const char* reason, int error_code)
		: error_code_(std::error_code(error_code, windows_category()))
	{
		what_ = exception::get_format_string("%s : %s", reason ? reason : error_code_.category().name(), error_code_.message().c_str());
	}

	const std::error_code& windows_exception::code() const
	{
		return error_code_;
	}

}
