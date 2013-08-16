#pragma once

#include <slk/utility/buffer.h>
#include "WesTable.hpp"

namespace slk
{
	struct WesReader
	{
		static void Read(buffer_reader& reader, WesTable& table);
	};
}
