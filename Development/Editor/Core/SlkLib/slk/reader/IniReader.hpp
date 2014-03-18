#pragma once

#include <base/util/buffer.h>
#include <slk/table/IniTable.hpp>

namespace slk
{
	class IniReader
	{
	public:
		static void Read(base::util::buffer_reader& reader, IniTable& table);
	};
}
