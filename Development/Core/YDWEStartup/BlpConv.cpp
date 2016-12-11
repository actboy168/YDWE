#include <Windows.h>
#include <BlpConv/BlpConv.h>
#include <base/file/stream.h>

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
