#pragma once

#include <slk/utility/buffer.h>
#include "IniTable.hpp"

namespace slk
{
	class IniReader
	{
	public:
		static void Read(buffer_reader& reader, IniTable& table);
	};
}
