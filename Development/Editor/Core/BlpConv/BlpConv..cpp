#include "BlpConv.h"
#include "Blp.h"
#include "Bmp.h"
#include <Windows.h>
#include <GdiPlus.h>   
#pragma comment(lib, "Gdiplus.lib")

namespace IMAGE
{
	struct picture_t
	{
		BUFFER buffer;
		int    width;
		int    height;
	};

	bool Blp2Bmp(const BUFFER& SourceBuffer, BUFFER& TargetBuffer)
	{
		picture_t pic;

		if (!BLP().Read(SourceBuffer, pic.buffer, &pic.width, &pic.height))
		{
			return false;
		}

		if (!BMP().Write(pic.buffer, TargetBuffer, pic.width, pic.height, 0))
		{
			return false;
		}

		return true;
	}

	bool Bmp2Blp(const BUFFER& SourceBuffer, BUFFER& TargetBuffer)
	{
		picture_t pic;

		if (!BMP().Read(SourceBuffer, pic.buffer, &pic.width, &pic.height))
		{
			return false;
		}

		if (!BLP().Write(pic.buffer, TargetBuffer, pic.width, pic.height, 100))
		{
			return false;
		}

		return true;
	}

	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;

	bool BlpBlend(const BUFFER& BlpA, const BUFFER& BlpB, BUFFER& BlpC)
	{
		Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

		picture_t a, b;

		if (!BLP().Read(BlpA, a.buffer, &a.width, &a.height))
		{
			return false;
		}
		if (!BLP().Read(BlpB, b.buffer, &b.width, &b.height))
		{
			return false;
		}

		if (!BLP().Write(b.buffer, BlpC, 64, 64, 100))
		{
			return false;
		}
		return true;
	}
}
