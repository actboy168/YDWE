#include "BlpConv.h"
#include "Blp.h"
#include "Bmp.h"
#include <Windows.h>

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
}
