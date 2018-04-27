#include <base/hook/replace_pointer.h>
#include <base/hook/detail/memory_protect.h>
#include <Windows.h>

namespace base { namespace hook {
	uintptr_t replace_pointer(uintptr_t address, uintptr_t new_value)
	{
		uintptr_t old_value = 0;

		detail::memory_protect protect_(address);
		if (protect_.success())
		{
			DWORD written = 0;
			if (!::ReadProcessMemory(::GetCurrentProcess(), (LPCVOID)address, &old_value, sizeof(uintptr_t), &written))
			{
				return 0;
			}

			if (!::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, &new_value, sizeof(uintptr_t), &written))
			{
				return 0;
			}

			::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, sizeof(uintptr_t));
		}

		return old_value;
	}
}}
