#include <Shlwapi.h>
#include <stdio.h>
#include <tchar.h>

extern CHAR g_szDllPath[MAX_PATH];

BOOL g_bDisableSaveLoadSystem = TRUE;

BOOL GetYDTriggerImportFilePath(OUT LPTSTR lpszDest)
{
  return (NULL != PathCombine(lpszDest, g_szDllPath, TEXT("YDTrigger\\Import.h")));
}

BOOL CreateYDTriggerImportFile()
{
  TCHAR szYDTriggerImportFilePath[MAX_PATH];
  FILE* fp;

  if (!GetYDTriggerImportFilePath(szYDTriggerImportFilePath))
    return FALSE;

  fp = _tfopen(szYDTriggerImportFilePath, TEXT("w"));
  if (NULL == fp)
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