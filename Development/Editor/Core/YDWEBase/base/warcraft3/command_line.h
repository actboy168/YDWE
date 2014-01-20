#pragma once

#include <base/config.h>
#include <functional>
#include <string>

namespace base { namespace warcraft3 {
	_BASE_API void parse_command_line(std::function<void(std::wstring const&, std::wstring const&)> proc);
}}

