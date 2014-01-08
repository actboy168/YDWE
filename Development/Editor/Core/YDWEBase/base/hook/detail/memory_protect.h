#pragma once

#include <base/config.h>
#include <Windows.h>

namespace base { namespace hook { namespace detail {
	class memory_protect
	{
	public:
		memory_protect(uintptr_t address, size_t size = sizeof(uintptr_t));
		~memory_protect();
		bool success() const;

	private:
		bool      success_;
		DWORD     access_;
		uintptr_t address_;
		size_t    size_;
	};
}}}
