#pragma once

#include <base/util/buffer.h>
#include <slk/table/SlkTable.hpp>

namespace slk
{
	struct SLKLIB_API SlkReader
	{
		static void Read(base::util::buffer_reader& reader, SlkTable& table);
	};
}
