#pragma once

#include <Windows.h>

namespace ydwe { namespace hook { namespace detail {
	class memory_protect
	{
	public:
		memory_protect(uintptr_t address);
		~memory_protect();
		bool success() const;

	private:
		bool  success_;
		MEMORY_BASIC_INFORMATION mbi_;
		DWORD access_;
	};
}}}
