#pragma once

#include "Buffer.hpp"
#include "MetaTable.hpp"

namespace slk
{
	struct MetaReader
	{
		static void Read(buffer_reader& reader, MetaTable& table);
	};
}