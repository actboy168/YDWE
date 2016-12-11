#include "png_utils.h"
#include <png.h>
#include <pngstruct.h>
#include <pnginfo.h>

namespace png_utils
{
	void PNGAPI user_error_fn(png_structp png, png_const_charp sz) { }
	void PNGAPI user_warning_fn(png_structp png, png_const_charp sz) { }

	struct png_read {
		const char* buf;
		size_t len;
		size_t offset;
	};

	static void png_read_func(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		png_read* pr = (png_read*)png_get_io_ptr(png_ptr);
		memcpy(data, pr->buf + pr->offset, length);
		pr->offset += length;
	}

	img_t load(const char* buf, size_t len)
	{
		HBITMAP hbm = NULL;

		if (len <= 8)
			return { 0, 0, 0 };

		bool retVal = false;
		int size = len;

		if (png_sig_cmp((png_const_bytep)buf, 0, 8))
			return{ 0, 0, 0 };

		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, user_error_fn, user_warning_fn);
		if (!png_ptr)
			return{ 0, 0, 0 };
		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
			return{ 0, 0, 0 };
		}

		png_infop end_info = png_create_info_struct(png_ptr);
		if (!end_info)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
			return{ 0, 0, 0 };
		}

		png_read pr = { buf, len, 0 };
		png_set_read_fn(png_ptr, (void*)&pr, png_read_func);
		png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_BGR, NULL);
		png_bytep* row_pointers = png_get_rows(png_ptr, info_ptr);

		int width = info_ptr->width;
		int height = info_ptr->height;
		int bpp = info_ptr->channels * 8;
		int memWidth = (width * (bpp >> 3) + 3) & ~3;

		LPBITMAPINFO lpbi = (LPBITMAPINFO) new char[sizeof(BITMAPINFOHEADER) + (256 * sizeof(RGBQUAD))];

		for (int a_i = 0; a_i < 256; a_i++)
		{
			lpbi->bmiColors[a_i].rgbRed = (BYTE)a_i;
			lpbi->bmiColors[a_i].rgbGreen = (BYTE)a_i;
			lpbi->bmiColors[a_i].rgbBlue = (BYTE)a_i;
			lpbi->bmiColors[a_i].rgbReserved = 0;
		}

		lpbi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		lpbi->bmiHeader.biWidth = width;
		lpbi->bmiHeader.biHeight = -height;
		lpbi->bmiHeader.biPlanes = 1;
		lpbi->bmiHeader.biBitCount = bpp;
		lpbi->bmiHeader.biCompression = BI_RGB;
		lpbi->bmiHeader.biSizeImage = memWidth * height;
		lpbi->bmiHeader.biXPelsPerMeter = 0;
		lpbi->bmiHeader.biYPelsPerMeter = 0;
		lpbi->bmiHeader.biClrUsed = 0;
		lpbi->bmiHeader.biClrImportant = 0;

		BYTE * pixelData;
		hbm = CreateDIBSection(NULL, lpbi, DIB_RGB_COLORS, (void **)&pixelData, NULL, 0);
		if (hbm && pixelData)
		{
			for (int i = 0; i < height; i++)
				memcpy(pixelData + memWidth * i, row_pointers[i], width * info_ptr->channels);
		}

		if (bpp == 32)
		{
			for (int y = 0; y < height; y++)
			{
				BYTE *pPixel = (BYTE *)pixelData + width * 4 * y;
				for (int x = 0; x < width; x++)
				{
					pPixel[0] = pPixel[0] * pPixel[3] / 255;
					pPixel[1] = pPixel[1] * pPixel[3] / 255;
					pPixel[2] = pPixel[2] * pPixel[3] / 255;
					pPixel += 4;
				}
			}
		}

		delete (char*)lpbi;
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		return{ hbm, width, height };
	}
}
