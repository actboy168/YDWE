#pragma once

#include <slk/utility/buffer.h>
#include "WtsTable.hpp"

namespace slk
{
	struct WtsReader
	{
		static void Read(buffer_reader& reader, WtsTable& table);
	};
}
