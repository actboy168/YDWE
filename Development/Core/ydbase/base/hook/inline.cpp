#include <base/hook/inline.h>
#include <base/hook/detail/replace_pointer.h>
#include <Windows.h>

#if defined(DISABLE_DETOURS)
#include <MinHook.h>
#else
#include <detours.h>
#endif

namespace base { namespace hook {

#if defined(DISABLE_DETOURS)
	struct minhook {
		minhook() { MH_Initialize(); }
		~minhook() { MH_Uninitialize(); }
	};
	minhook mh;
	struct hook_info {
		uintptr_t  target;
	};
	bool install(uintptr_t* pointer_ptr, uintptr_t detour, hook_t* ph)
	{
		uintptr_t target = *pointer_ptr;
		MH_STATUS status;
		if ((status = MH_CreateHook((PVOID)target, (PVOID)detour, (PVOID*)pointer_ptr)) == MH_OK) {
			if ((status = MH_EnableHook((PVOID)target)) == MH_OK) {
				if (ph) {
					hook_info* hi = new hook_info;
					hi->target = target;
					*ph = hi;
				}
				return true;
			}
		}
		::SetLastError(status);
		return false;
	}

	bool uninstall(hook_t* ph)
	{
		if (!ph || !*ph) {
			::SetLastError((DWORD)MH_UNKNOWN);
			return false;
		}
		hook_info* hi = *(hook_info**)ph;
		uintptr_t target = hi->target;
		MH_STATUS status;
		if ((status = MH_DisableHook((PVOID)target)) == MH_OK) {
			if ((status = MH_RemoveHook((PVOID)target)) == MH_OK) {
				delete hi;
				*ph = 0;
				return true;
			}
		}
		::SetLastError(status);
		return false;
	}
#else
	struct hook_info {
		uintptr_t* pointer_ptr;
		uintptr_t  detour;
	};
	bool install(uintptr_t* pointer_ptr, uintptr_t detour, hook_t* ph)
	{
		LONG status;
		if ((status = DetourTransactionBegin()) == NO_ERROR) {
			if ((status = DetourUpdateThread(::GetCurrentThread())) == NO_ERROR) {
				if ((status = DetourAttach((PVOID*)pointer_ptr, (PVOID)detour)) == NO_ERROR) {
					if ((status = DetourTransactionCommit()) == NO_ERROR) {
						if (ph) {
							hook_info* hi = new hook_info;
							hi->detour = detour;
							hi->pointer_ptr = pointer_ptr;
							*ph = hi;
						}
						return true;
					}
				}
			}
			DetourTransactionAbort();
		}
		::SetLastError(status);
		return false;
	}

	bool uninstall(hook_t* ph)
	{
		if (!ph || !*ph) {
			::SetLastError(ERROR_INVALID_HANDLE);
			return false;
		}
		hook_info* hi = *(hook_info**)ph;
		LONG status;
		if ((status = DetourTransactionBegin()) == NO_ERROR) {
			if ((status = DetourUpdateThread(::GetCurrentThread())) == NO_ERROR) {
				if ((status = DetourDetach((PVOID*)hi->pointer_ptr, (PVOID)hi->detour)) == NO_ERROR) {
					if ((status = DetourTransactionCommit()) == NO_ERROR) {
						delete hi;
						*ph = 0;
						return true;
					}
				}
			}
			DetourTransactionAbort();
		}
		::SetLastError(status);
		return false;
	}
#endif

	uintptr_t replace_pointer(uintptr_t address, uintptr_t new_value)
	{
		return detail::replace_pointer(address, new_value);
	}
}}
