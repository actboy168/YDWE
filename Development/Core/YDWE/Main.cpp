#include <windows.h>
#include <shlwapi.h>
#include <string>

void InstallEnv(HINSTANCE module)
{
	/* 获得几个要加到Path的文件夹路径 */
	wchar_t binaryPath[MAX_PATH];
	wchar_t pathEnvironmentVariable[32767 /* http://msdn.microsoft.com/en-us/library/windows/desktop/ms683188.aspx */];

	// 当前exe路径
	GetModuleFileNameW(module, binaryPath, sizeof(binaryPath) / sizeof(binaryPath[0]));
	PathRemoveBlanksW(binaryPath);
	PathUnquoteSpacesW(binaryPath);
	PathRemoveBackslashW(binaryPath);

	// bin文件夹路径
	PathRemoveFileSpecW(binaryPath);
	PathAppendW(binaryPath, L"bin");

	// 拷贝到待增加路径字符串
	std::wstring newPathEnvironmentVariable = binaryPath;

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
}

typedef INT (WINAPI *TPFStartup)(HINSTANCE currentInstance, HINSTANCE previousInstance, LPSTR pCommandLine, INT showType);

INT WINAPI WinMain(HINSTANCE currentInstance, HINSTANCE previousInstance, LPSTR pCommandLine, INT showType)
{
	InstallEnv(currentInstance);

	INT res = -1;
	HMODULE m = LoadLibraryW(L"YDWEStartup.dll");
	if (m)
	{
		TPFStartup pfStartup = (TPFStartup)GetProcAddress(m, "YDWEStartup");
		if (pfStartup) {
			res = pfStartup(currentInstance, previousInstance, pCommandLine, showType);
		}
		FreeLibrary(m);
	}
	return res;
}
