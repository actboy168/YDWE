#pragma once

#include <Windows.h>	

namespace png_utils
{
	struct img_t {
		HBITMAP bmp;
		int w;
		int h;
	};
	img_t load(const char* buf, size_t len);
}
