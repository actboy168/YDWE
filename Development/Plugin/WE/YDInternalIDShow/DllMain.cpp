#include <windows.h>
#include <base/hook/fp_call.h>
#include <base/hook/inline.h>

namespace NYDWEPlugin
{

static uintptr_t fpgSlkStringGet;
static BOOL __fastcall SlkStringGetHook(void *thisObject, void *dummy, int rowId, size_t index, char *buffer, size_t bufferSize)
{
	BOOL result = base::this_call<BOOL>(fpgSlkStringGet, thisObject, rowId, index, buffer, bufferSize);

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

static void InitHook()
{
	fpgSlkStringGet = (uintptr_t)0x00515670;

	base::hook::inline_install(&fpgSlkStringGet, (uintptr_t)SlkStringGetHook);
}

static void CleanHook()
{
	base::hook::inline_uninstall(&fpgSlkStringGet, (uintptr_t)SlkStringGetHook);
}

HMODULE gSelfModule;

} // namespace NYDWEPlugin

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	using namespace NYDWEPlugin;

	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(module);
		gSelfModule = module;

		InitHook();
		break;
	case DLL_PROCESS_DETACH:
		CleanHook();
		break;
	default:
		break;
	}

	return TRUE;
}
