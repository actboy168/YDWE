
#include "Common.h"
#include "Core/CC_Put.h"

int _fastcall Utf8toAscii(char src[], char dst[], unsigned int limit);

void ShowError(DWORD OutClass, LPCSTR szErrorText)
{
	static CHAR szBuf[260];

	GetWEString(szErrorText, szBuf, 260, 0);
	Utf8toAscii(szBuf, szBuf, 260);
	PUT_CONST(szBuf, 1);
}

#include <strsafe.h>

void ShowErrorN(DWORD OutClass, LPCSTR szErrorText, ...)
{
	static CHAR szTmp[260];
	static CHAR szBuf[260];
	va_list va;

	GetWEString(szErrorText, szTmp, 260, 0);
	Utf8toAscii(szTmp, szTmp, 260);

	va_start(va, szErrorText);
	StringCchVPrintfA(szBuf, 260, szTmp, va);
	va_end(va);

	PUT_CONST(szBuf, 1);
}
