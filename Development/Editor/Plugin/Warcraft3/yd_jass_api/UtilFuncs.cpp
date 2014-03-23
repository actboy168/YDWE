#include <base/hook/fp_call.h>
#include <base/hook/iat.h>
#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/version.h>
#include <base/warcraft3/jass.h>
#include <cassert>

namespace base { namespace warcraft3 { namespace japi {

	uint32_t __cdecl EXGetUnitId(uint32_t unit_handle)
	{
		uintptr_t unit_object = handle_to_object(unit_handle);
		if (!unit_object)
		{
			return 0;
		}
		assert('+w3u' == get_object_type(unit_object));
		uint32_t retval = *(uint32_t*)(unit_object + 0x30);
		return retval;
	}

	void __cdecl EXSetUnitId(uint32_t unit_handle, uint32_t value)
	{
		uintptr_t unit_object = handle_to_object(unit_handle);
		if (!unit_object)
		{
			return ;
		}
		assert('+w3u' == get_object_type(unit_object));
		*(uint32_t*)(unit_object + 0x30) = value;
	}

	void __cdecl EXRevealTrainingProcess(bool flag)
	{
		war3_searcher&s = get_war3_searcher();
		uintptr_t address_base = s.base();
		DWORD dwOldProtect;
		LPVOID address_ability = (LPVOID*)(address_base + 0x34F2A6);
		LPVOID address_cooldown = (LPVOID*)(address_base + 0x34F2E6);
		LPVOID address_progressbar = (LPVOID*)(address_base + 0x28E1DC);
		LPVOID address_resourceview = (LPVOID*)(address_base + 0x35FA4A);
		VirtualProtect(address_ability, sizeof(uint16_t), PAGE_EXECUTE_READWRITE, &dwOldProtect);
		VirtualProtect(address_cooldown, sizeof(uint16_t), PAGE_EXECUTE_READWRITE, &dwOldProtect);
		VirtualProtect(address_progressbar, sizeof(uint16_t), PAGE_EXECUTE_READWRITE, &dwOldProtect);
		VirtualProtect(address_resourceview, sizeof(uint16_t), PAGE_EXECUTE_READWRITE, &dwOldProtect);

		if (flag)
		{
			// modify
			*(uint16_t*)address_ability = 0x02EB;	// ability
			*(uint16_t*)address_cooldown = 0x02EB;	// cooldown
			*(uint16_t*)address_progressbar = 0x33EB;	// progress bar
			*(uint16_t*)address_resourceview = 0x9090;	// resource view
		}
		else
		{
			// restore
			*(uint16_t*)address_ability = 0xC085;	// ability
			*(uint16_t*)address_cooldown = 0xC085;	// cooldown
			*(uint16_t*)address_progressbar = 0xC085;  // progress bar
			*(uint16_t*)address_resourceview = 0x08EB;	// resource view
		}
	}

	uint32_t __cdecl EXGetGameBaseAdr()
	{
		war3_searcher&s = get_war3_searcher();
		uintptr_t address = s.base();
		return address;
	}


	void InitializeUtilFuncs()
	{
		jass::japi_add((uintptr_t)EXGetUnitId, "EXGetUnitId", "(Hunit;)I");
		jass::japi_add((uintptr_t)EXSetUnitId, "EXSetUnitId", "(Hunit;I)V");

		jass::japi_add((uintptr_t)EXRevealTrainingProcess, "EXRevealTrainingProcess", "(B)V");
		jass::japi_add((uintptr_t)EXGetGameBaseAdr, "EXGetGameBaseAdr", "()I");
	}
}}}