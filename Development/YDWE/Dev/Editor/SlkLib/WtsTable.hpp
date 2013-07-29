#pragma once

#include "BaseTable.hpp"

namespace slk
{
	class WtsTable : public HashTable<uint32_t, std::string>::Type
	{
	};
}
