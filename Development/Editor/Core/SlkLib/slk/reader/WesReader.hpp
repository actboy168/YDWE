#pragma once

#include <base/util/buffer.h>
#include <slk/table/WesTable.hpp>

namespace slk
{
	struct WesReader
	{
		static void Read(base::buffer_reader& reader, WesTable& table);
	};
}
