#include <base/win/env_variable.h>
#include <base/exception/windows_exception.h>
#include <Windows.h>
#include <array>

namespace base { 

namespace win {

	env_variable::env_variable(const wchar_t* name)
		: name_(name)
	{ }

	env_variable::env_variable(std::wstring const& name)
		: name_(name)
	{ }

	env_variable::env_variable(std::wstring&& name)
		: name_(std::move(name))
	{ }

	//
	// see the
	// http://msdn.microsoft.com/en-us/library/windows/desktop/ms683188.aspx
	//
	std::wstring env_variable::get()
	{
		std::array<wchar_t, 32767> buffer;
		buffer[0] = L'\0';

		DWORD retval = ::GetEnvironmentVariableW(name_.c_str(), buffer.data(), buffer.size());
		if (retval == 0 || retval > buffer.size())
		{
			throw windows_exception("GetEnvironmentVariable failed.");
		}

		return std::move(std::wstring(buffer.begin(), buffer.end()));
	}

	std::optional<std::wstring> env_variable::get_nothrow() throw()
	{
		std::array<wchar_t, 32767> buffer;
		buffer[0] = L'\0';

		DWORD retval = ::GetEnvironmentVariableW(name_.c_str(), buffer.data(), buffer.size());
		if (retval == 0 || retval > buffer.size())
		{
			return std::move(std::optional<std::wstring>());
		}

		return std::move(std::optional<std::wstring>(std::wstring(buffer.begin(), buffer.begin() + retval)));
	}

	bool env_variable::set(std::wstring const& value)
	{
		return FALSE != ::SetEnvironmentVariableW(name_.c_str(), value.c_str());
	}

	bool env_variable::del()
	{
		return FALSE != ::SetEnvironmentVariableW(name_.c_str(), NULL);
	}
}

}
