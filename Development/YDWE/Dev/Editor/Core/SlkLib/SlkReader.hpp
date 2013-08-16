#pragma once

#include <slk/utility/buffer.h>
#include "SlkTable.hpp"

namespace slk
{
	struct SLKLIB_API SlkReader
	{
		static void Read(buffer_reader& reader, SlkTable& table);
	};
}
