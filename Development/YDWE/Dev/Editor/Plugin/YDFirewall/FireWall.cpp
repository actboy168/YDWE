#include <winsock2.h>
#include <windows.h>

#include "FireWall.h"
#include "Win7FireWall.h"
#include "WinXPSP2FireWall.h"

class COM
{
public:
	COM() :hrComInit(S_OK) 
	{ 
		hrComInit = ::CoInitializeEx(0, COINIT_APARTMENTTHREADED);

		if (hrComInit != RPC_E_CHANGED_MODE)
		{
			if (FAILED(hrComInit))
			{
				throw hrComInit;
			}
		}
	}

	~COM() 
	{
		if (SUCCEEDED(hrComInit))
		{
			::CoUninitialize();
		}
	}
private:
	HRESULT hrComInit;
};

class NET
{
public:
	NET()
	{
		int err = WSAStartup(MAKEWORD(1, 1), &wsaData);
		if (0 != err)
		{
			throw err;
		}

		if (LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1)
		{
			WSACleanup();
			throw err;
		}
	}

	~NET()
	{
		WSACleanup();
	}
private:
	WSADATA wsaData;

};

DWORD GetOSMajorVersion()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);
	return osvi.dwMajorVersion;
}

BOOL FireWallAddApplication(LPCWSTR lpszProcessImageFileName, LPCWSTR lpszRegisterName)
{
	try
	{
		COM _Com;

		if (GetOSMajorVersion() >= 6)
		{
			// see http://msdn.microsoft.com/en-us/library/windows/desktop/aa366418(v=vs.85).aspx
			Win7FireWall fireWall;

			return fireWall.AddApplication(
					lpszProcessImageFileName, 
					lpszRegisterName);
		}
		else
		{
			WinXPSP2FireWall fireWall;
			if (FW_NOERROR == fireWall.Initialize())
			{
				if (FW_NOERROR == fireWall.AddApplication(
					lpszProcessImageFileName, 
					lpszRegisterName))
				{
					return TRUE;
				}
			}

			return FALSE;
		}
	}
	catch (...) 
	{
		return FALSE;
	}
}
