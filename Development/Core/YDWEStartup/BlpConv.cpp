#include <Windows.h>
#include <base/file/stream.h>
#include <BlpConv/BlpConv.h>

namespace image
{
	struct picture_t
	{
		pixels pixels;
		int    width;
		int    height;
	};

	bool Blp2Bmp(const buffer& SourceBuffer, buffer& TargetBuffer)
	{
		picture_t pic;

		if (!blp::read(SourceBuffer, pic.pixels, &pic.width, &pic.height))
		{
			return false;
		}

		if (!bmp::write(pic.pixels, TargetBuffer, pic.width, pic.height, 0))
		{
			return false;
		}

		return true;
	}

	bool Bmp2Blp(const buffer& SourceBuffer, buffer& TargetBuffer)
	{
		picture_t pic;

		if (!bmp::read(SourceBuffer, pic.pixels, &pic.width, &pic.height))
		{
			return false;
		}

		if (!blp::write(pic.pixels, TargetBuffer, pic.width, pic.height, 100))
		{
			return false;
		}

		return true;
	}
}

bool Blp2Bmp(const wchar_t* blp, const wchar_t* bmp)
{
	image::buffer input = base::file::read_stream(blp).read<image::buffer>();
	image::buffer output;
	if (!image::Blp2Bmp(input, output))
	{
		return false;
	}	   
	base::file::write_stream(bmp).write(output);
	return true;
}
