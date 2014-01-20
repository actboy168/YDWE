#pragma once

#include <base/config.h>
#include <functional>
#include <map>
#include <string>

namespace base { namespace warcraft3 {

#pragma warning (push)
#pragma warning (disable:4251)
	class _BASE_API command_line
		: public std::map<std::wstring, std::wstring>
	{
	public:
		command_line(); 
		std::wstring str() const;

	public:
		static void parse(std::function<void(std::wstring const&, std::wstring const&)> proc);
	};
#pragma warning (pop)
}}

