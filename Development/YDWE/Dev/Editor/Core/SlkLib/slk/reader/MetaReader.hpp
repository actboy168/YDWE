#pragma once

#include <slk/utility/buffer.h>
#include <slk/table/MetaTable.hpp>

namespace slk
{
	struct MetaReader
	{
		static void Read(buffer_reader& reader, MetaTable& table);
	};
}