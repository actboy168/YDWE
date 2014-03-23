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

	void InitializeUtilFuncs()
	{
		jass::japi_add((uintptr_t)EXGetUnitId, "EXGetUnitId", "(Hunit;)I");
		jass::japi_add((uintptr_t)EXSetUnitId, "EXSetUnitId", "(Hunit;I)V");
	}
}}}