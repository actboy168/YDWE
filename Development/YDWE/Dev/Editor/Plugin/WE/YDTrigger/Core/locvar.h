#pragma once

#include <Windows.h>

namespace locvar
{
	void get(DWORD This, DWORD OutClass, char* type_name);
	void set(DWORD This, DWORD OutClass, char* type_name);
}
