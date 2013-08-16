#pragma once

#include <slk/utility/buffer.h>
#include "MetaTable.hpp"

namespace slk
{
	struct MetaReader
	{
		static void Read(buffer_reader& reader, MetaTable& table);
	};
}