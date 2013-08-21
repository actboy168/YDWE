#pragma once

#include "Buffer.hpp"
#include "WtsTable.hpp"

namespace slk
{
	struct WtsReader
	{
		static void Read(buffer&& buf, WtsTable& table);
	};
}
