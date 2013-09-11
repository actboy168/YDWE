#pragma once

#include <base/config.h>
#include <Windows.h>

namespace base { 
namespace hook { namespace detail {
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
}}
}
