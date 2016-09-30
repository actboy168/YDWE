#pragma once

#include <base/util/buffer.h>
#include <slk/table/WtsTable.hpp>

namespace slk
{
	struct WtsReader
	{
		static void Read(base::buffer_reader& reader, WtsTable& table);
	};
}
