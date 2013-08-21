#include <ydwe/hook/detail/memory_protect.h>

namespace ydwe { namespace hook { namespace detail {

	memory_protect::memory_protect(uintptr_t address)
		: success_(false)
	{
		if (0 == ::VirtualQuery((LPCVOID)address, &mbi_, sizeof(mbi_)))
			return ;


		if (!::VirtualProtect(mbi_.BaseAddress, mbi_.RegionSize, PAGE_WRITECOPY, &access_))
		{
			if (!::VirtualProtect(mbi_.BaseAddress, mbi_.RegionSize, PAGE_READWRITE, &access_))
			{
				return ;
			}
		}

		success_ = true;
	}

	memory_protect::~memory_protect()
	{
		if (success())
		{
			DWORD newAccess;
			::VirtualProtect(mbi_.BaseAddress, mbi_.RegionSize, access_, &newAccess);
		}
	}

	bool memory_protect::success() const
	{
		return success_;
	}
}}}
