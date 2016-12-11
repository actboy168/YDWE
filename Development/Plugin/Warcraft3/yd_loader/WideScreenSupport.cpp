#include <base/warcraft3/version.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/hook/inline.h>
#include <base/hook/fp_call.h>
#include <stdint.h>
#include "DllModule.h"

namespace WideScreen 
{
	static uintptr_t fake = 0;

	static int __fastcall real(int32_t ecx, int32_t edx, float a2, float a3, float a4, float a5)
	{
		RECT r;
		if (::GetWindowRect(g_DllMod.hWar3Wnd, &r))
		{
			float fWideScreenMul = ((float)(r.right - r.left) / (float)(r.bottom - r.top)) / (4.0f / 3.0f);
			a2 *= fWideScreenMul;
			a3 *= fWideScreenMul;
		}
		return base::fast_call<int>(fake, ecx, edx, a2, a3, a4, a5);
	}

	static uintptr_t search()
	{
		using namespace base::warcraft3;

		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = 0;
		if (s.get_version() <= version_120e)
		{
			ptr = s.search_string("E:\\Drive1\\temp\\buildwar3x\\War3\\Source\\UI\\CWorldFrameWar3.cpp");
			ptr += 4;
			ptr = next_opcode(ptr, 0xC7, 6);
			ptr += 6;
			ptr = next_opcode(ptr, 0xC7, 6);
			ptr += 6;
			ptr = next_opcode(ptr, 0xC7, 6);
			ptr += 2;
			ptr = *(uintptr_t*)ptr;
		}
		else
		{
			ptr = get_vfn_ptr(".?AVCWorldFrameWar3@@");
		}
		ptr += 0x2C;
		ptr = *(uintptr_t*)ptr;
		uint8_t m = s.get_version() <= version_120e ? 0x53 : 0x51;
		for (;;)
		{
			ptr = next_opcode(ptr, 0x57, 1);
			ptr += 1;
			if (*(uint8_t*)ptr == m)
			{
				ptr = next_opcode(ptr, 0xE8, 5);
				ptr = convert_function(ptr);
				break;
			}
		}
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr += 5;
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr = convert_function(ptr);

		ptr = next_opcode(ptr, 0xE8, 5);
		ptr = convert_function(ptr);

		ptr = next_opcode(ptr, 0xE8, 5);

		if (s.get_version() >= version_127a)
		{
			ptr += 5;
			ptr = next_opcode(ptr, 0xE8, 5);
		}

		ptr = convert_function(ptr);
		return ptr;
	}

	void initialize()
	{
		static uintptr_t address = search();
		fake = address;
		base::hook::inline_install(&fake, (uintptr_t)real);
	}
}
