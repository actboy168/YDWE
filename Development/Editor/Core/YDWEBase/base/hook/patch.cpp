#include <base/hook/patch.h>
#include <base/hook/detail/memory_protect.h>
#include <Windows.h>

namespace base { namespace hook {

	bool patch(uintptr_t address, const char* value, size_t value_size)
	{
		detail::memory_protect protect_(address, value_size);
		if (protect_.success())
		{
			DWORD written;
			if (::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, value, value_size, &written))
			{
				::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, value_size);
				return true;
			}
		}
		return false;
	}
}}
