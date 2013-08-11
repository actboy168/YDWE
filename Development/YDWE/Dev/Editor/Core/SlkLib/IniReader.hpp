#pragma once

#include "Buffer.hpp"
#include "IniTable.hpp"

namespace slk
{
	class IniReader
	{
	public:
		static void Read(buffer_reader& reader, IniTable& table);
	};
}
