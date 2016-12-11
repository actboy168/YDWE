/*
 * YDWE启动
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <locale>
#include <windows.h>
#include <shlwapi.h>

typedef INT (WINAPI *TPFStartup)(HINSTANCE currentInstance, HINSTANCE previousInstance, LPSTR pCommandLine, INT showType);

/** \brief 主函数
 *
 * 主启动函数。负责调用启动DLL的启动函数
 */
INT WINAPI WinMain(HINSTANCE currentInstance, HINSTANCE previousInstance, LPSTR pCommandLine, INT showType)
{
	INT exitCode = -1;

	/* 获得几个要加到Path的文件夹路径 */
	wchar_t binaryPath[MAX_PATH];
	wchar_t pathEnvironmentVariable[32767 /* http://msdn.microsoft.com/en-us/library/windows/desktop/ms683188.aspx */ ];
	std::wstring newPathEnvironmentVariable;

	// 当前exe路径
	GetModuleFileNameW(currentInstance, binaryPath, sizeof(binaryPath) / sizeof(binaryPath[0]));
	PathRemoveBlanksW(binaryPath);
	PathUnquoteSpacesW(binaryPath);
	PathRemoveBackslashW(binaryPath);

	// bin文件夹路径
	PathRemoveFileSpecW(binaryPath);
	PathAppendW(binaryPath, L"bin");

	// 拷贝到待增加路径字符串
	newPathEnvironmentVariable.append(binaryPath);

	// plugin文件夹路径
	PathRemoveFileSpecW(binaryPath);
	PathAppendW(binaryPath, L"plugin");

	// 拷贝到待增加路径字符串
	newPathEnvironmentVariable.append(L";");
	newPathEnvironmentVariable.append(binaryPath);

	/* 设置PATH环境变量 */
	// 获取环境变量
	pathEnvironmentVariable[0] = L'\0';
	DWORD length = GetEnvironmentVariableW(L"PATH", pathEnvironmentVariable, sizeof(pathEnvironmentVariable) / sizeof(pathEnvironmentVariable[0]));

	// 组合新环境变量
	if (length > 0)
		newPathEnvironmentVariable.append(L";");

	newPathEnvironmentVariable.append(pathEnvironmentVariable);

	// 拷贝回去
	wcscpy_s(pathEnvironmentVariable, newPathEnvironmentVariable.c_str());

	// 设置环境变量
	SetEnvironmentVariableW(L"PATH", pathEnvironmentVariable);

	/* 载入CRT的Dll，防止Dll冲突 */

	// 再次得到bin文件夹路径
	PathRemoveFileSpecW(binaryPath);
	PathAppendW(binaryPath, L"bin");

#ifdef _DEBUG
#if _MSC_VER == 1800  
	wchar_t msvcr_dll[] = L"msvcr120d.dll";
	wchar_t msvcp_dll[] = L"msvcp120d.dll";
#else 
	wchar_t msvcr_dll[] = L"msvcr100d.dll";
	wchar_t msvcp_dll[] = L"msvcp100d.dll"; 
#endif
#else
#if _MSC_VER == 1800  
	wchar_t msvcr_dll[] = L"msvcr120.dll";
	wchar_t msvcp_dll[] = L"msvcp120.dll";
#else 
	wchar_t msvcr_dll[] = L"msvcr100.dll";
	wchar_t msvcp_dll[] = L"msvcp100.dll";
#endif
#endif
	PathAppendW(binaryPath, msvcr_dll);
	HMODULE msvcr100 = LoadLibraryW(binaryPath);
	PathRemoveFileSpecW(binaryPath);
	PathAppendW(binaryPath, msvcp_dll);
	HMODULE msvcp100 = LoadLibraryW(binaryPath);

	// 尝试载入YDWEStartup.dll
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
