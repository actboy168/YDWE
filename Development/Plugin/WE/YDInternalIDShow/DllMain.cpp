#include <windows.h>
#include <base/hook/fp_call.h>
#include <base/hook/inline.h>

namespace base { namespace hook {
	template <class T>
	struct inline_helper
	{
		static uintptr_t real;
		static bool hook(uintptr_t address) { real = address; return hook(); }
		static bool hook() { return inline_install(&T::real, (uintptr_t)T::fake); }
		static bool unhook() { return inline_uninstall(&T::real, (uintptr_t)T::fake); }
	};
	template <class T>
	uintptr_t inline_helper<T>::real = 0;
}}


struct SlkStringGet
	: public base::hook::inline_helper<SlkStringGet>
{
	static BOOL __fastcall fake(void *thisObject, void *dummy, int rowId, size_t index, char *buffer, size_t bufferSize)
	{
		BOOL result = base::this_call<BOOL>(real, thisObject, rowId, index, buffer, bufferSize);

		void *pNode;
		__asm
		{
			mov		eax, thisObject;
			mov		edx, [eax + 400];
			mov		eax, index;
			imul	eax, 108;
			add		eax, edx;
			mov		pNode, eax;
		}

		char *fileName = (char *)thisObject + 8;
		char *itemName = (char *)pNode + 8;

		size_t length = strlen(buffer);
		if (length + 7 <= bufferSize &&
			strcmp(itemName, "displayName") == 0 && (
				strcmp(fileName, "Units\\AbilityMetaData") == 0 ||
				strcmp(fileName, "Units\\UnitMetaData") == 0 ||
				strcmp(fileName, "Units\\AbilityBuffMetaData") == 0 ||
				strcmp(fileName, "Units\\UpgradeMetaData") == 0 ||
				strcmp(fileName, "Units\\DestructableMetaData") == 0 ||
				strcmp(fileName, "Doodads\\DoodadMetaData") == 0
				)
			)
		{
			buffer[length] = '[';
			*(int *)&buffer[length + 1] = rowId;
			buffer[length + 5] = ']';
			buffer[length + 6] = 0;
		}

		return result;
	}
};

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(module);
		SlkStringGet::hook(0x00515670);
		break;
	case DLL_PROCESS_DETACH:
		SlkStringGet::unhook();
		break;
	default:
		break;
	}

	return TRUE;
}
