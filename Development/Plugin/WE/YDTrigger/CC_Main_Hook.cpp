#include "Common.h"

BOOL CreateYDTriggerImportFile();

extern BOOL g_bDisableSaveLoadSystem;

void _fastcall
CC_Main_Hook(DWORD OutClass)
{
	g_bDisableSaveLoadSystem = TRUE;

	CC_Main(OutClass);

	CreateYDTriggerImportFile();
}
