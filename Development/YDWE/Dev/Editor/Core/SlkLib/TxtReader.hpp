#pragma once

#include "Buffer.hpp"
#include "SlkTable.hpp"

namespace slk
{
	struct SLKLIB_API TxtReader
	{
		static void Read(buffer_reader& reader, SlkTable& table);
	};
}
