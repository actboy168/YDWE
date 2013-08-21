#pragma once

#include <string>

namespace slk
{
	struct InterfaceStorm
	{
		typedef int error_code;

		virtual bool has(std::string const& path) = 0;
		virtual std::string load(std::string const& path, error_code& ec) = 0;
	};
}
