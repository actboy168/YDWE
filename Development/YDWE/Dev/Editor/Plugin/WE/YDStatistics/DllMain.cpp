#include <windows.h>
#include "Singleton.h"
#include "RemoteLog.h"
#include "Ini/IniReader.hpp"

class CModule
{
public:
	CModule()
	{
		Singleton<RemoteLog>::instance().start();
	}

	~CModule()
	{
		Singleton<RemoteLog>::instance().stop();
	}
};

CModule _Module;

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (DLL_PROCESS_ATTACH == dwReason)
	{
		::DisableThreadLibraryCalls(hInstance);
	}
	else if (DLL_PROCESS_DETACH == dwReason)
	{
	}

	return TRUE;
}

extern "C" BOOL WINAPI Post(PostType* pType, PostData* pData)
{
	Singleton<RemoteLog>::instance().post_to_server(pType, pData);
	return TRUE;
}

static BOOL ReadConfigData(LPCSTR lpszFileName, PostData& data)
{
	try
	{
		using namespace SLK;
		Buffer buf(File(lpszFileName).Reads());
		IniReader::Read(buf, 
			[&](const std::string& szSection, const std::string& szKey, const std::string& szValue)
		{
			data.insert(std::make_pair(szSection + "_" + szKey, szValue));
		});
	}
	catch (SLK::Exception& e)
	{
		e.what();
		return FALSE;
	}

	return TRUE;
}

extern "C" BOOL WINAPI PostConfig(LPCSTR lpszFileName)
{
	if (!lpszFileName)
		return FALSE;

	PostType type = "ydwe_config";
	PostData data;

	if (!ReadConfigData(lpszFileName, data))
	{
		return FALSE;
	}

	Post(&type, &data);
	return TRUE;
}

const char *PluginName()
{
	return "YDStatistics";
}
