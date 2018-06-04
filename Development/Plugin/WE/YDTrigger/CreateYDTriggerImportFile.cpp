#include <stdio.h>
#include <base/path/get_path.h>

BOOL g_bDisableSaveLoadSystem = TRUE;

BOOL CreateYDTriggerImportFile()
{
	fs::path file = base::path::get(base::path::DIR_MODULE).parent_path() / L"YDTrigger" / L"Import.h";

	FILE* fp = NULL;
	errno_t err = _wfopen_s(&fp, file.c_str(), L"w");
	if (err != 0 || NULL == fp)
		return FALSE;

	fprintf(fp, "#ifndef INCLUDE_IMPORT_H\n");
	fprintf(fp, "#define INCLUDE_IMPORT_H\n");

	if (g_bDisableSaveLoadSystem)
	{
		fprintf(fp, "#\n");
		fprintf(fp, "#  ifndef DISABLE_SAVE_LOAD_SYSTEM\n");
		fprintf(fp, "#    define DISABLE_SAVE_LOAD_SYSTEM\n");
		fprintf(fp, "#  endif\n");
		fprintf(fp, "#\n");
	}

	fprintf(fp, "#endif\n");

	fclose(fp);
	return TRUE;
}
