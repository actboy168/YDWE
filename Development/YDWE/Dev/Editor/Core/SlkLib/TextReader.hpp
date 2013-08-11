#pragma once

#include "Buffer.hpp"
#include "Util.hpp"
#include <functional>

namespace slk
{
	struct TextReader
	{
		static void RemoveBom(buffer_reader& reader);
		static void EachLine(buffer_reader& reader, std::function<void(std::string&)> callback);
	};
}
