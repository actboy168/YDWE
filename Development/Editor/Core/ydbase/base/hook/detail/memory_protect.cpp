#include <base/hook/detail/memory_protect.h>

namespace base { namespace hook { namespace detail {

	memory_protect::memory_protect(uintptr_t address, size_t size)
		: success_(false)
		, access_(0)
		, address_(address)
		, size_(size)
	{
		if (!::VirtualProtect((LPVOID)address_, size_, PAGE_READWRITE, &access_))
		{
			return;
		}

		success_ = true;
	}

	memory_protect::~memory_protect()
	{
		if (success())
		{
			DWORD new_access;
			::VirtualProtect((LPVOID)address_, size_, access_, &new_access);
		}
	}

	bool memory_protect::success() const
	{
		return success_;
	}
}}}
