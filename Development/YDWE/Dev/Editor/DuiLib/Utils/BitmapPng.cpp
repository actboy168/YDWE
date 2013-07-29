
#include "stdafx.h"
#include "libpng\png.h"
#include "BitmapPng.h"

namespace DuiLib
{
	class stream_t
	{
	public:
		stream_t(const uint8_t* buffer, size_t size)
			: buffer_(buffer)
			, size_(size)
			, cur_(buffer_)
			, end_(buffer_+size_)
		{ }

		size_t read(uint8_t* buffer, size_t size)
		{
			size_t real_size = 0;
			if (cur_ + size > end_)
			{
				real_size = end_ - cur_; 
			}
			else
			{
				real_size = size;
			}

			memcpy(buffer, cur_, real_size);
			cur_ += real_size;
			return real_size;
		}

	private:
		const uint8_t* buffer_;
		size_t         size_;
		const uint8_t* cur_;
		const uint8_t* end_;
	};

	class png_t
	{
	public:
		png_t();
		bool    from_file(stream_t* pFile);
		HBITMAP to_bmp();

	private:
		struct color_t
		{
			uint8_t red;
			uint8_t green;
			uint8_t blue;

			color_t(uint8_t r, uint8_t g, uint8_t b)
				: red(r)
				, green(g)
				, blue(b)
			{ }
		};

		std::unique_ptr<uint8_t[]> pbImage;
		uint32_t                   cxImgSize;
		uint32_t                   cyImgSize;
		uint32_t                   cImgChannels;
		color_t                    bkgColor;

	private:
		static void PNGAPI png_read_data(png_structp png_ptr, png_bytep data, png_size_t length);
		static void PNGAPI png_cexcept_error(png_structp png_ptr, png_const_charp msg);
	};

	void PNGAPI png_t::png_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
	{
		stream_t* pFile = (stream_t*)png_ptr->io_ptr;

		png_size_t sizeRead = (png_size_t)pFile->read(data, (UINT)length);

		if (sizeRead != length)
		{
			throw std::exception("CFileException::endOfFile");
		}
	}

	void PNGAPI png_t::png_cexcept_error(png_structp /*png_ptr*/, png_const_charp /*msg*/)
	{
		throw std::exception("CFileException::badSeek");
	}

	png_t::png_t()
		: bkgColor(127, 127, 127)
	{ }

	bool png_t::from_file(stream_t* pFile)
	{
		png_byte            pbSig[8];
		int                 iBitDepth;
		int                 iColorType;
		double              dGamma;
		png_color_16       *pBackground;
		png_uint_32         ulRowBytes;
		std::unique_ptr<png_bytep[]> ppbRowPointers;
		png_uint_32         i;
		png_structp         png_ptr = NULL;
		png_infop           info_ptr = NULL;

		// open the PNG input file

		this->pbImage.release();

		if (!pFile)
		{
			return false;
		}

		pFile->read(pbSig, 8);

		if (!png_check_sig(pbSig, 8))
		{
			return false;
		}

		// create the two png(-info) structures

		png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, (png_error_ptr)png_cexcept_error, (png_error_ptr)NULL);
		if (!png_ptr)
		{
			return false;
		}

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, NULL, NULL);
			return false;
		}

		try
		{

			// initialize the png structure

			png_set_read_fn(png_ptr, (png_voidp)pFile, png_read_data);

			png_set_sig_bytes(png_ptr, 8);

			// read all PNG info up to image data

			png_read_info(png_ptr, info_ptr);

			// get width, height, bit-depth and color-type

			png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *)&this->cxImgSize, (png_uint_32 *)&this->cyImgSize, &iBitDepth, &iColorType, NULL, NULL, NULL);

			// expand images of all color-type and bit-depth to 3x8 bit RGB images
			// let the library process things like alpha, transparency, background

			if (iBitDepth == 16)
				png_set_strip_16(png_ptr);
			if (iColorType == PNG_COLOR_TYPE_PALETTE)
				png_set_expand(png_ptr);
			if (iBitDepth < 8)
				png_set_expand(png_ptr);
			if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
				png_set_expand(png_ptr);
			if (iColorType == PNG_COLOR_TYPE_GRAY ||
				iColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
				png_set_gray_to_rgb(png_ptr);

			// set the background color to draw transparent and alpha images over.
			if (png_get_bKGD(png_ptr, info_ptr, &pBackground))
			{
				png_set_background(png_ptr, pBackground, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
				this->bkgColor.red   = (byte) pBackground->red;
				this->bkgColor.green = (byte) pBackground->green;
				this->bkgColor.blue  = (byte) pBackground->blue;
			}
			else
			{
			}

			// if required set gamma conversion
			if (png_get_gAMA(png_ptr, info_ptr, &dGamma))
				png_set_gamma(png_ptr, (double) 2.2, dGamma);

			// after the transformations have been registered update info_ptr data

			png_read_update_info(png_ptr, info_ptr);

			// get again width, height and the new bit-depth and color-type

			png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *)&this->cxImgSize, (png_uint_32 *)&this->cyImgSize, &iBitDepth, &iColorType, NULL, NULL, NULL);


			// row_bytes is the width x number of channels

			ulRowBytes = png_get_rowbytes(png_ptr, info_ptr);
			this->cImgChannels = png_get_channels(png_ptr, info_ptr);

			// now we can allocate memory to store the image
			this->pbImage.reset(new png_byte[ulRowBytes * this->cyImgSize]);


			// and allocate memory for an array of row-pointers
			ppbRowPointers.reset(new png_bytep[this->cyImgSize]);

			// set the individual row-pointers to point at the correct offsets

			for (i = 0; i < this->cyImgSize; i++)
				ppbRowPointers[i] = &this->pbImage[i * ulRowBytes];

			// now we can go ahead and just read the whole image

			png_read_image(png_ptr, ppbRowPointers.get());

			// read the additional chunks in the PNG file (not really needed)

			png_read_end(png_ptr, NULL);
		}
		catch (std::exception& /*e*/)
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

			this->pbImage.release();

			return false;
		}

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		return true;
	}

	HBITMAP png_t::to_bmp()
	{
		uint32_t xImg, yImg;
		BYTE *src, *dst;
		BYTE r, g, b, a;
		BITMAPINFO bmi;
		LPBYTE pBits;
		HBITMAP hbm;
		const int cDIChannels = 4;
		UINT wImgRowBytes;
		UINT wDIRowBytes;

		// Initialize header to 0s.
		ZeroMemory(&bmi, sizeof(bmi));

		// Fill out the fields you care about.
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = this->cxImgSize;
		bmi.bmiHeader.biHeight = -(int32_t)this->cyImgSize;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		hbm = ::CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, (LPVOID*)&pBits, NULL, 0);

		if (!hbm || !pBits)
		{
			return NULL;
		}
		wImgRowBytes = this->cImgChannels * this->cxImgSize;
		wDIRowBytes = cDIChannels * this->cxImgSize;

		// copy image to screen

		for (yImg = 0; yImg < this->cyImgSize; yImg++)
		{
			src = &this->pbImage[yImg * wImgRowBytes];
			dst = pBits + yImg * wDIRowBytes;

			for (xImg = 0; xImg < this->cxImgSize; xImg++)
			{
				r = *src++;
				g = *src++;
				b = *src++;
				*dst++ = b;
				*dst++ = g;
				*dst++ = r;
				a = 0;

				if (this->cImgChannels == 4)
				{
					a = *src++;
				}
				*dst++ = a;
			}
		}

		return hbm;
	}

	CBitmapPng::CBitmapPng()
	{ }

	BOOL CBitmapPng::LoadFromMemory(BYTE* pBuffer, DWORD dwSize)
	{
		if (!pBuffer || dwSize < 10)
			return FALSE;

		stream_t file(pBuffer, dwSize);
		png_t png;

		if (!png.from_file(&file))
		{
			return FALSE;
		}

		HBITMAP hBitmap = png.to_bmp();

		if (!hBitmap)
			return FALSE;

		return TRUE;
	}
}
