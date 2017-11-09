#include <windows.h>
#include <base/hook/patch.h>
#include <base/warcraft3/war3_searcher.h>  
#include <base/warcraft3/version.h>

template <size_t n>
void patch_all(const char(&from)[n], const char(&to)[n])
{
	auto& s = base::warcraft3::get_war3_searcher();
	auto [frist, last] = s.text();
	for (uintptr_t ptr = frist; ptr < last - (n - 1); ++ptr)
	{
		if (0 == memcmp((const void*)ptr, from, n - 1))
		{
			base::hook::patch(ptr, to);
			ptr += (n - 2);
		}
	}
}

void Initialize()
{
	if (base::warcraft3::get_war3_searcher().get_version() >= base::warcraft3::version_127a)
	{
		return;
	}
	else if (base::warcraft3::get_war3_searcher().get_version() >= base::warcraft3::version_124b)
	{
		patch_all("\x3D\x00\x00\x80\x00", "\x3D\xFF\xFF\xFF\xFF");
	}
	else
	{
		patch_all("\x3D\x00\x00\x40\x00", "\x3D\xFF\xFF\xFF\xFF");
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}
