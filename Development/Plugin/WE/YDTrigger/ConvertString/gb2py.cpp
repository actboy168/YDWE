#include <windows.h>
#include <ctype.h>
#include "../Storm/StormAdapter.h"
#include "ConvertString.h"


int _fastcall 
	Utf8toAscii(char src[], char dst[], unsigned int limit)
{
	wchar_t* temp;
	unsigned int len;

	len = MultiByteToWideChar(CP_UTF8, 0, src, -1, 0, 0);

	temp = (wchar_t*)malloc(len*sizeof(wchar_t));

	if (temp == NULL) 
		return 0;

	if (len > 0)
	{
		len = MultiByteToWideChar(CP_UTF8, 0, src, -1, temp, len*sizeof(wchar_t));
		len = WideCharToMultiByte(CP_ACP, 0, temp, -1, 0,  0, 0, 0);
		if (len > limit) len = limit;
		WideCharToMultiByte(CP_ACP, 0, temp, -1, dst, len*sizeof(char), 0, 0);

		dst[len] = 0;
		free(temp);
		return 1;
	}
	else
	{
		free(temp);
		return 0;
	}
}
