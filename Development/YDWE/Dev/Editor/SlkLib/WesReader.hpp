#pragma once

#include "Buffer.hpp"
#include "WesTable.hpp"

namespace slk
{
	struct WesReader
	{
		static void Read(buffer&& buf, WesTable& table);
	};
}
