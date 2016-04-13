/*
 * YDWE����
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <locale>
#include <windows.h>
#include <shlwapi.h>

typedef INT (WINAPI *TPFStartup)(HINSTANCE currentInstance, HINSTANCE previousInstance, LPSTR pCommandLine, INT showType);

/** \brief ������
 *
 * �����������������������DLL����������
 */
INT WINAPI WinMain(HINSTANCE currentInstance, HINSTANCE previousInstance, LPSTR pCommandLine, INT showType)
{
	INT exitCode = -1;

	/* ��ü���Ҫ�ӵ�Path���ļ���·�� */
	wchar_t binaryPath[MAX_PATH];
	wchar_t pathEnvironmentVariable[32767 /* http://msdn.microsoft.com/en-us/library/windows/desktop/ms683188.aspx */ ];
	std::wstring newPathEnvironmentVariable;

	// ��ǰexe·��
	GetModuleFileNameW(currentInstance, binaryPath, sizeof(binaryPath) / sizeof(binaryPath[0]));
	PathRemoveBlanksW(binaryPath);
	PathUnquoteSpacesW(binaryPath);
	PathRemoveBackslashW(binaryPath);

	// bin�ļ���·��
	PathRemoveFileSpecW(binaryPath);
	PathAppendW(binaryPath, L"bin");

	// ������������·���ַ���
	newPathEnvironmentVariable.append(binaryPath);

	// plugin�ļ���·��
	PathRemoveFileSpecW(binaryPath);
	PathAppendW(binaryPath, L"plugin");

	// ������������·���ַ���
	newPathEnvironmentVariable.append(L";");
	newPathEnvironmentVariable.append(binaryPath);

	/* ����PATH�������� */
	// ��ȡ��������
	pathEnvironmentVariable[0] = L'\0';
	DWORD length = GetEnvironmentVariableW(L"PATH", pathEnvironmentVariable, sizeof(pathEnvironmentVariable) / sizeof(pathEnvironmentVariable[0]));

	// ����»�������
	if (length > 0)
		newPathEnvironmentVariable.append(L";");

	newPathEnvironmentVariable.append(pathEnvironmentVariable);

	// ������ȥ
	wcscpy_s(pathEnvironmentVariable, newPathEnvironmentVariable.c_str());

	// ���û�������
	SetEnvironmentVariableW(L"PATH", pathEnvironmentVariable);

	/* ����CRT��Dll����ֹDll��ͻ */

	// �ٴεõ�bin�ļ���·��
	PathRemoveFileSpecW(binaryPath);
	PathAppendW(binaryPath, L"bin");

#ifdef _DEBUG
#if _MSC_VER == 1900  
	wchar_t msvcr_dll[] = L"msvcr120d.dll";
	wchar_t msvcp_dll[] = L"msvcp140d.dll";
#else
	wchar_t msvcr_dll[] = L"msvcr120d.dll";
	wchar_t msvcp_dll[] = L"msvcp120d.dll"; 
#endif
#else
#if _MSC_VER == 1900  
	wchar_t msvcr_dll[] = L"msvcr120.dll";
	wchar_t msvcp_dll[] = L"msvcp140.dll";
#else 
	wchar_t msvcr_dll[] = L"msvcr120.dll";
	wchar_t msvcp_dll[] = L"msvcp120.dll";
#endif
#endif
	PathAppendW(binaryPath, msvcr_dll);
	HMODULE msvcr100 = LoadLibraryW(binaryPath);
	PathRemoveFileSpecW(binaryPath);
	PathAppendW(binaryPath, msvcp_dll);
	HMODULE msvcp100 = LoadLibraryW(binaryPath);

	// ��������YDWEStartup.dll
	HMODULE startupModule = LoadLibraryW(L"YDWEStartup.dll");
	if (startupModule)
	{
		TPFStartup pfStartup = (TPFStartup)GetProcAddress(startupModule, "YDWEStartup");
		if (pfStartup)
			exitCode = pfStartup(currentInstance, previousInstance, pCommandLine, showType);

		FreeLibrary(startupModule);
	}

	if (msvcp100)
		FreeLibrary(msvcp100);
	if (msvcr100)
		FreeLibrary(msvcr100);

	return exitCode;
}
