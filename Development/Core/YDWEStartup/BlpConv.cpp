#include <Windows.h>
#include <base/file/stream.h>
#include <BlpConv/Blp.h>
#include <BlpConv/Bmp.h>

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

		if (!BLP::Read(SourceBuffer, pic.buffer, &pic.width, &pic.height))
		{
			return false;
		}

		if (!BMP::Write(pic.buffer, TargetBuffer, pic.width, pic.height, 0))
		{
			return false;
		}

		return true;
	}

	bool Bmp2Blp(const BUFFER& SourceBuffer, BUFFER& TargetBuffer)
	{
		picture_t pic;

		if (!BMP::Read(SourceBuffer, pic.buffer, &pic.width, &pic.height))
		{
			return false;
		}

		if (!BLP::Write(pic.buffer, TargetBuffer, pic.width, pic.height, 100))
		{
			return false;
		}

		return true;
	}
}

bool Blp2Bmp(const wchar_t* blp, const wchar_t* bmp)
{
	IMAGE::BUFFER input = base::file::read_stream(blp).read<IMAGE::BUFFER>();
	IMAGE::BUFFER output;
	if (!IMAGE::Blp2Bmp(input, output))
	{
		return false;
	}	   
	base::file::write_stream(bmp).write(output);
	return true;
}
