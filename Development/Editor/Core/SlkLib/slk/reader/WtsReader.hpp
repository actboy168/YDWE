#pragma once

#include <slk/utility/buffer.h>
#include <slk/table/WtsTable.hpp>

namespace slk
{
	struct WtsReader
	{
		static void Read(buffer_reader& reader, WtsTable& table);
	};
}
