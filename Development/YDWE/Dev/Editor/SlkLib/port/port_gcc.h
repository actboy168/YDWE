#pragma once

#include <cstdint>
#include <sstream>
#include <cstdio>

#ifndef _countof
template <typename _CountofType, size_t _SizeOfArray>
char (*__countof_helper(UNALIGNED _CountofType(&_Array)[_SizeOfArray]))[_SizeOfArray];
#define _countof(_Array) sizeof(*__countof_helper(_Array))
#endif

namespace std
{
	template <typename _Ty>
	inline string to_string(_Ty val)
	{
		stringstream s;
		s << val;
		return s.str();
	}
}

#define foreach(VAL, COL) for (VAL : COL)

