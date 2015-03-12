#pragma once

#include <base/util/buffer.h>
#include <slk/table/IniTable.hpp>

namespace slk
{
	class IniReader
	{
	public:
		static void Read(base::buffer_reader& reader, IniTable& table);
	};
}
