#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include "Win7FireWall.h"

bool Win7FireWall::AddApplication(const wchar_t *lpszProcessImageFileName, const wchar_t *lpszRegisterName)
{
	// netsh advfirewall firewall add rule name="allow messenger" dir=in program="c:\programfiles\messenger\msmsgs.exe¡± action=allow
	wchar_t buffer[MAX_PATH + 1024];

	swprintf_s(buffer, L"netsh advfirewall firewall add rule name=\"%s\" dir=in program=\"%s\" action=allow", lpszRegisterName, lpszProcessImageFileName);
	
	STARTUPINFOW startupInfo;
	GetStartupInfoW(&startupInfo);
	PROCESS_INFORMATION processInfo;

	if (CreateProcessW(NULL, buffer, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo))
	{
		WaitForSingleObject(processInfo.hProcess, INFINITE);

		DWORD code;
		BOOL result = GetExitCodeProcess(processInfo.hProcess, &code);

		CloseHandle(processInfo.hThread);
		CloseHandle(processInfo.hProcess);

		return result && code == 0;
	}
	else
		return false;
}