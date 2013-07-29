#include "stdafx.h"
#include "CPacket.hpp"
#include "CSocket.hpp"

HANDLE g_hEventFinish = NULL;
HANDLE g_hThread = NULL;

unsigned int WINAPI SocketThread(void *lpParameter)
{
	CSocket socket;
	if (!socket.open())
	{
		return 0;
	}

	CPacketKeepAlive packet((UINT32)lpParameter);
	
	while (TRUE)
	{
		socket.sendto(packet.GetData(), packet.GetSize());

		if (WAIT_TIMEOUT != ::WaitForSingleObject(g_hEventFinish, 1000))
		{
			break;
		}
	}

	socket.close();

	return 0;
}

extern "C" __declspec(dllexport) 
BOOL __cdecl Start(DWORD nMajor, DWORD nMinor)
{
	unsigned int dwThreadID = 0;

	g_hThread = (HANDLE)_beginthreadex(
		NULL,
		0,
		SocketThread,
		(PVOID)MAKELONG(nMajor, nMinor),
		0,
		&dwThreadID);

	return (NULL != g_hThread);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		{
			::DisableThreadLibraryCalls(hModule);

			g_hEventFinish = ::CreateEvent(NULL, TRUE, FALSE, NULL);
		}
		break;
	case DLL_PROCESS_DETACH:
		{
			if (g_hThread != NULL) 
			{
				::SetEvent(g_hEventFinish);
				if (::WaitForSingleObject(g_hThread, 300) != WAIT_ABANDONED)
				{
					::TerminateThread(g_hThread, -1);
					::CloseHandle(g_hThread);
				}
			}
			::CloseHandle(g_hEventFinish);
		}
		break;
	}
	return TRUE;
}

const char *PluginName()
{
	return "YDClient";
}
