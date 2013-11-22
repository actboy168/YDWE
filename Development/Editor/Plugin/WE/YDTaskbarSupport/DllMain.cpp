#include <windows.h>
#include <random>
#include <string>
#include <boost/filesystem.hpp>
#include <base/win/version.h>
#include <base/path/service.h>
#include "Windows7.h"
#include "JumpList.h"

namespace fs = ::boost::filesystem;

std::wstring const& CurrentAppID()
{
	static std::wstring strAppID = L"YDWE.WorldEdit." + std::to_wstring((long long)std::random_device()());
	return strAppID;
}

bool JumpListAddRecentTask(JumpList& jumpList, fs::path const& ydweDirectory, fs::path const& filePath)
{
	HRESULT hr = jumpList.AddTask(filePath.filename().c_str(), [&](CComPtr<IShellLinkW>& shellLinkPtr)
	{
		shellLinkPtr->SetPath((ydweDirectory / L"YDWE.exe").c_str());
		shellLinkPtr->SetArguments((L" -loadfile \"" + filePath.wstring() + L"\"").c_str());
		shellLinkPtr->SetDescription(filePath.c_str());
		shellLinkPtr->SetIconLocation((ydweDirectory / L"bin" / L"logo.ico").c_str(), 0);
	});

	return SUCCEEDED(hr);
}

void Initialize()
{
	if (base::win::get_version() < base::win::VERSION_WIN7)
	{
		return ;
	}

	windows7::SetCurrentProcessExplicitAppUserModelID(CurrentAppID().c_str());

	JumpList jumpList;
	jumpList.SetAppID(CurrentAppID().c_str());
	if (jumpList.InitializeList())
	{
		fs::path ydweDirectory = base::path::get(base::path::DIR_MODULE).remove_filename().remove_filename();

		jumpList.AddTaskSeparator();

		jumpList.AddTask(L"YDWEÅäÖÃ", [&](CComPtr<IShellLinkW>& shellLinkPtr)
		{
			shellLinkPtr->SetPath((ydweDirectory / L"bin" / L"YDWEConfig.exe").c_str());
			shellLinkPtr->SetDescription(L"´ò¿ªYDWEÅäÖÃ³ÌÐò¡£");
			shellLinkPtr->SetIconLocation((ydweDirectory / L"bin" / L"logo.ico").c_str(), 0);
		});
	}
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);
	}

	return TRUE;
}

const char *PluginName()
{
	return "YDTaskbarSupport";
}
