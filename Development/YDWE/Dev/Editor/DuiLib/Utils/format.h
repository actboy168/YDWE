#pragma once

#include <string>
#include <memory>

namespace util
{
	inline std::wstring format(std::wstring format, ...)
	{
		if (format.empty())
		{
			return std::move(std::wstring(L""));
		}

		va_list marker = NULL; 

		va_start(marker, format);
		size_t len = _vscwprintf(format.c_str(), marker) + 1;
		std::unique_ptr<wchar_t[]> buffer(new wchar_t[len]);
		int nWritten = _vsnwprintf_s(buffer.get(), len, len, format.c_str(), marker);
		va_end(marker); 

		if (nWritten > 0)
		{
			return std::move(std::wstring(buffer.get(), buffer.get()+len));
		}

		return std::move(std::wstring(L"")); 
	}

	inline std::string format( std::string format, ... )
	{
		if (format.empty())
		{
			return std::move(std::string(""));
		}

		std::string formattedStr("");

		va_list argsList = NULL; 

		va_start(argsList, format); 
		size_t len = _vscprintf(format.c_str(), argsList) + 1;
		std::unique_ptr<char[]> buffer(new char[len]);
		int nWritten = _vsnprintf_s(buffer.get(), len, len, format.c_str(), argsList);    
		va_end(argsList); 

		if (nWritten > 0)
		{
			return std::move(std::string(buffer.get(), buffer.get()+len));
		}

		return std::move(std::string("")); 
	}
}
