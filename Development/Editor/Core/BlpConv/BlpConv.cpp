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

	static inline unsigned char clamp_channel_bits8(int c)
	{
		if (c > 255)
			return 255;
		if (c < 0)
			return 0;
		return static_cast<unsigned char>(c);
	}

	static const size_t kBlpSize = 64;
	bool BlpBlend(const BUFFER& BlpA, const BUFFER& BlpB, BUFFER& BlpC)
	{
		picture_t a, b, c;
		if (!BLP().Read(BlpA, a.buffer, &a.width, &a.height))
		{
			return false;
		}
		if (a.width != kBlpSize || a.height != kBlpSize)
		{
			return false;
		}
		if (!BLP().Read(BlpB, b.buffer, &b.width, &b.height))
		{
			return false;
		}
		if (b.width != kBlpSize || b.height != kBlpSize)
		{
			return false;
		}
		c.width = kBlpSize;
		c.height = kBlpSize;
		c.buffer.Resize(kBlpSize * kBlpSize * 4);
		unsigned char* ptr = c.buffer.GetData();
		size_t n = 0;
		for (int y = 0; y < a.height; ++y)
		{
			for (int x = 0; x < a.width; ++x)
			{
				BLP_RGBA pixel_a = reinterpret_cast<BLP_RGBA*>(a.buffer.GetData())[n];
				BLP_RGBA pixel_b = reinterpret_cast<BLP_RGBA*>(b.buffer.GetData())[n];
				n += 1;
				*ptr++ = clamp_channel_bits8(((255 - pixel_a.Alpha) * pixel_b.Red + pixel_a.Red) / (255 * 2));
				*ptr++ = clamp_channel_bits8(((255 - pixel_a.Alpha) * pixel_b.Green + pixel_a.Green) / (255 * 2));
				*ptr++ = clamp_channel_bits8(((255 - pixel_a.Alpha) * pixel_b.Blue + pixel_a.Blue) / (255 * 2));
				*ptr++ = clamp_channel_bits8(255 - (255 - pixel_a.Alpha) * (255 - pixel_b.Alpha) / 255);
			}
		}
		//if (!BMP().Write(c.buffer, BlpC, c.width, c.height, 0))
		//{
		//	return false;
		//}
		if (!BLP().Write(c.buffer, BlpC, c.width, c.height, 95))
		{
			return false;
		}
		return true;
	}
}
