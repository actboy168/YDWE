#pragma once

#include <base/util/buffer.h>
#include <slk/table/MetaTable.hpp>

namespace slk
{
	struct MetaReader
	{
		static void Read(base::buffer_reader& reader, MetaTable& table, bool /*create_if_not_exists*/);
	};
}