

#pragma once


#include "Utils.h"

namespace DuiLib
{
	class  CBitmapPng
	{
	public:
		CBitmapPng();

	public:
		BOOL LoadFromMemory(BYTE* pBuffer, DWORD dwSize);
	};
}

