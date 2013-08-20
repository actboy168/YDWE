#pragma once

#include <slk/utility/buffer.h>
#include <slk/table/IniTable.hpp>

namespace slk
{
	class IniReader
	{
	public:
		static void Read(buffer_reader& reader, IniTable& table);
	};
}
