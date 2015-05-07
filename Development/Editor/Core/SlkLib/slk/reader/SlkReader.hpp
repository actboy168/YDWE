#pragma once

#include <base/util/buffer.h>
#include <slk/table/SlkTable.hpp>

namespace slk
{
	struct SLKLIB_API SlkReader
	{
		static void Read(base::buffer_reader& reader, SlkTable& table, bool /*create_if_not_exists*/);
	};
}
