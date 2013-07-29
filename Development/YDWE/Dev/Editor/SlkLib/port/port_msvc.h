#pragma once

#include <cstdint>

#define foreach(VAL, COL) \
	__pragma(warning(push)) \
	__pragma(warning(disable: 4239)) \
	for each (VAL in COL) \
	__pragma(warning(pop))
