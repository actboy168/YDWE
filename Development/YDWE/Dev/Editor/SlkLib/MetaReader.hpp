#pragma once

#include "Buffer.hpp"
#include "MetaTable.hpp"

namespace slk
{
	struct MetaReader
	{
		static void Read(buffer&& buf, MetaTable& table);
	};
}