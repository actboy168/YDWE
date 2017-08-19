#include "BlpConv.h"
#include <windows.h>
#include <memory>

namespace image { namespace bmp {

#define BITMAP_SIGNATURE 0x4d42

#pragma pack(push, 1)

typedef struct _BITMAP_FILEHEADER {
	uint16_t Signature;
	uint32_t Size;
	uint32_t Reserved;
	uint32_t BitsOffset;
} BITMAP_FILEHEADER;

typedef struct _BITMAP_HEADER {
	uint32_t HeaderSize;
	int32_t Width;
	int32_t Height;
	uint16_t Planes;
	uint16_t BitCount;
	uint32_t Compression;
	uint32_t SizeImage;
	int32_t PelsPerMeterX;
	int32_t PelsPerMeterY;
	uint32_t ClrUsed;
	uint32_t ClrImportant;
	uint32_t RedMask;
	uint32_t GreenMask;
	uint32_t BlueMask;
	uint32_t AlphaMask;
	uint32_t CsType;
	uint32_t Endpoints[9]; // see http://msdn2.microsoft.com/en-us/library/ms536569.aspx
	uint32_t GammaRed;
	uint32_t GammaGreen;
	uint32_t GammaBlue;
} BITMAP_HEADER;

typedef struct _BITMAP {
	BITMAP_FILEHEADER head;
	BITMAP_HEADER info;
} BITMAP;

typedef struct _BGRA {
	uint8_t Blue;
	uint8_t Green;
	uint8_t Red;
	uint8_t Alpha;
} BGRA;

#pragma pack(pop)

class CColor {
public:
	static inline unsigned int BitCountByMask(unsigned int Mask) {
		unsigned int BitCount = 0;
		while (Mask) {
			Mask &= Mask - 1;
			BitCount++;
		}
		return BitCount;
	}

	static inline unsigned int BitPositionByMask(unsigned int Mask) {
		return BitCountByMask((Mask & (~Mask + 1)) - 1);
	}

	static inline unsigned int ComponentByMask(unsigned int Color, unsigned int Mask) {
		unsigned int Component = Color & Mask;
		return Component >> BitPositionByMask(Mask);
	}

	static inline unsigned int BitCountToMask(unsigned int BitCount) {
		return (BitCount == 32) ? 0xFFFFFFFF : (1 << BitCount) - 1;
	}

	static unsigned int Convert(unsigned int Color, unsigned int FromBitCount, unsigned int ToBitCount) {
		if (ToBitCount < FromBitCount) {
			Color >>= (FromBitCount - ToBitCount);
		}
		else {
			Color <<= (ToBitCount - FromBitCount);
			if (Color > 0) {
				Color |= BitCountToMask(ToBitCount - FromBitCount);
			}
		}
		return Color;
	}
};

bool GetBits(void* Buffer, const pixels& input, unsigned int &Size, unsigned int width, unsigned int height, unsigned int RedMask, unsigned int GreenMask, unsigned int BlueMask, unsigned int AlphaMask, bool IncludePadding = true) {
	bool Result = false;

	uint32_t BitCountRed = CColor::BitCountByMask(RedMask);
	uint32_t BitCountGreen = CColor::BitCountByMask(GreenMask);
	uint32_t BitCountBlue = CColor::BitCountByMask(BlueMask);
	uint32_t BitCountAlpha = CColor::BitCountByMask(AlphaMask);

	unsigned int BitCount = (BitCountRed + BitCountGreen + BitCountBlue + BitCountAlpha + 7) & ~7;

	if (BitCount > 32) {
		return false;
	}

	unsigned int w = width;
	unsigned int dataBytesPerLine = (w * BitCount + 7) / 8;
	unsigned int LineWidth = (dataBytesPerLine + 3) & ~3;

	if (Size == 0 || Buffer == 0) {
		Size = (width * height * BitCount) / 8 + sizeof(unsigned int);
		return true;
	}

	uint8_t* BufferPtr = (uint8_t*)Buffer;

	Result = true;

	uint32_t BitPosRed = CColor::BitPositionByMask(RedMask);
	uint32_t BitPosGreen = CColor::BitPositionByMask(GreenMask);
	uint32_t BitPosBlue = CColor::BitPositionByMask(BlueMask);
	uint32_t BitPosAlpha = CColor::BitPositionByMask(AlphaMask);

	unsigned int j = 0;

	for (unsigned int i = 0; i < width * height; i++) {
		*(uint32_t*)BufferPtr =
			(CColor::Convert(input[i].b, 8, BitCountBlue) << BitPosBlue) |
			(CColor::Convert(input[i].g, 8, BitCountGreen) << BitPosGreen) |
			(CColor::Convert(input[i].r, 8, BitCountRed) << BitPosRed) |
			(CColor::Convert(input[i].a, 8, BitCountAlpha) << BitPosAlpha);

		if (IncludePadding) {
			j++;
			if (j >= w) {
				for (unsigned int k = 0; k < LineWidth - dataBytesPerLine; k++) {
					BufferPtr += (BitCount >> 3);
				}
				j = 0;
			}
		}

		BufferPtr += (BitCount >> 3);
	}

	Size -= sizeof(unsigned int);

	return Result;
}

bool GetBitsWithPalette(void* Buffer, const pixels& input, unsigned int &Size, unsigned int width, unsigned int height, unsigned int BitCount, BGRA* &Palette, unsigned int &PaletteSize, bool OptimalPalette = false, bool IncludePadding = true) {
	bool Result = false;

	if (BitCount > 16) {
		return false;
	}

	unsigned int w = width;
	unsigned int dataBytesPerLine = (w * BitCount + 7) / 8;
	unsigned int LineWidth = (dataBytesPerLine + 3) & ~3;

	if (Size == 0 || Buffer == 0) {
		Size = (LineWidth * height * BitCount) / 8;
		return true;
	}


	if (OptimalPalette) {
		PaletteSize = 0;
		// Not implemented
	}
	else {
		if (BitCount == 1) {
			PaletteSize = 2;
			// Not implemented: Who need that?
		}
		else if (BitCount == 4) { // 2:2:1
			PaletteSize = 16;
			Palette = new BGRA[PaletteSize];
			for (int r = 0; r < 4; r++) {
				for (int g = 0; g < 2; g++) {
					for (int b = 0; b < 2; b++) {
						Palette[r | g << 2 | b << 3].Red = r ? (r << 6) | 0x3f : 0;
						Palette[r | g << 2 | b << 3].Green = g ? (g << 7) | 0x7f : 0;
						Palette[r | g << 2 | b << 3].Blue = b ? (b << 7) | 0x7f : 0;
						Palette[r | g << 2 | b << 3].Alpha = 0xff;
					}
				}
			}
		}
		else if (BitCount == 8) { // 3:3:2
			PaletteSize = 256;
			Palette = new BGRA[PaletteSize];
			for (int r = 0; r < 8; r++) {
				for (int g = 0; g < 8; g++) {
					for (int b = 0; b < 4; b++) {
						Palette[r | g << 3 | b << 6].Red = r ? (r << 5) | 0x1f : 0;
						Palette[r | g << 3 | b << 6].Green = g ? (g << 5) | 0x1f : 0;
						Palette[r | g << 3 | b << 6].Blue = b ? (b << 6) | 0x3f : 0;
						Palette[r | g << 3 | b << 6].Alpha = 0xff;
					}
				}
			}
		}
		else if (BitCount == 16) { // 5:5:5
								   // Not implemented
		}
	}

	unsigned int j = 0;
	uint8_t* BufferPtr = (uint8_t*)Buffer;

	for (unsigned int i = 0; i < width * height; i++) {
		if (BitCount == 1) {
			// Not implemented: Who needs that?
		}
		else if (BitCount == 4) {
			*BufferPtr = ((input[i].r >> 6) | (input[i].g >> 7) << 2 | (input[i].b >> 7) << 3) << 4;
			i++;
			*BufferPtr |= (input[i].r >> 6) | (input[i].g >> 7) << 2 | (input[i].b >> 7) << 3;
		}
		else if (BitCount == 8) {
			*BufferPtr = (input[i].r >> 5) | (input[i].g >> 5) << 3 | (input[i].b >> 5) << 6;
		}
		else if (BitCount == 16) {
			// Not implemented
		}

		if (IncludePadding) {
			j++;
			if (j >= w) {
				for (unsigned int k = 0; k < (LineWidth - dataBytesPerLine); k++) {
					BufferPtr += BitCount / 8;
				}
				j = 0;
			}
		}

		BufferPtr++;
	}

	Result = true;

	return Result;
}

bool write(const pixels& input, buffer& output, unsigned int width, unsigned int height, unsigned int bitcount) {
	bool Result = true;

	BITMAP bmp = { 0 };
	bmp.head.Signature = BITMAP_SIGNATURE;
	bmp.head.BitsOffset = sizeof(BITMAP);
	bmp.info.HeaderSize = sizeof(BITMAP_HEADER);
	bmp.info.BitCount = bitcount;

	if (bitcount == 32) {
		bmp.info.Compression = 3;
		bmp.info.AlphaMask = 0xff000000;
		bmp.info.BlueMask = 0x000000ff;
		bmp.info.GreenMask = 0x0000ff00;
		bmp.info.RedMask = 0x00ff0000;
	}
	else if (bitcount == 16) {
		bmp.info.Compression = 3;
		bmp.info.AlphaMask = 0x00000000;
		bmp.info.BlueMask = 0x0000001f;
		bmp.info.GreenMask = 0x000007E0;
		bmp.info.RedMask = 0x0000F800;
	}
	else {
		bmp.info.Compression = 0;
	}

	unsigned int LineWidth = (width + 3) & ~3;

	bmp.info.Planes = 1;
	bmp.info.Height = height;
	bmp.info.Width = width;
	bmp.info.SizeImage = (LineWidth * bitcount * height) / 8;
	bmp.info.PelsPerMeterX = 3780;
	bmp.info.PelsPerMeterY = 3780;
	bmp.head.Size = bmp.info.SizeImage + bmp.head.BitsOffset;

	if (bitcount == 32) {
		output.resize(sizeof(BITMAP) + bmp.info.SizeImage);
		memcpy(output.data(), (char*)&bmp, sizeof(BITMAP));
		memcpy(output.data() + sizeof(BITMAP), input.data(), bmp.info.SizeImage);
	}
	else if (bitcount < 16) {
		std::unique_ptr<uint8_t> Bitmap(new uint8_t[bmp.info.SizeImage]);
		BGRA *Palette = 0;
		unsigned int PaletteSize = 0;
		if (GetBitsWithPalette(Bitmap.get(), input, bmp.info.SizeImage, width, height, bitcount, Palette, PaletteSize)) {
			bmp.head.BitsOffset += PaletteSize * sizeof(BGRA);
			bmp.head.Size = bmp.info.SizeImage + bmp.head.BitsOffset;
			output.resize(sizeof(BITMAP) + PaletteSize * sizeof(BGRA) + bmp.info.SizeImage);
			memcpy(output.data(), (char*)&bmp, sizeof(BITMAP));
			memcpy(output.data() + sizeof(BITMAP), Palette, PaletteSize * sizeof(BGRA));
			memcpy(output.data() + bmp.head.BitsOffset, Bitmap.get(), bmp.info.SizeImage);
		}
		delete[] Palette;
	}
	else {
		uint32_t RedMask = 0;
		uint32_t GreenMask = 0;
		uint32_t BlueMask = 0;
		uint32_t AlphaMask = 0;

		if (bitcount == 16) {
			RedMask = 0x0000F800;
			GreenMask = 0x000007E0;
			BlueMask = 0x0000001F;
			AlphaMask = 0x00000000;
		}
		else if (bitcount == 24) {
			RedMask = 0x00FF0000;
			GreenMask = 0x0000FF00;
			BlueMask = 0x000000FF;
		}
		else {
			// Other color formats are not valid
			Result = false;
		}

		if (Result) {
			if (GetBits(NULL, input, bmp.info.SizeImage, width, height, RedMask, GreenMask, BlueMask, AlphaMask)) {
				std::unique_ptr<uint8_t> Bitmap(new uint8_t[bmp.info.SizeImage]);
				if (GetBits(Bitmap.get(), input, bmp.info.SizeImage, width, height, RedMask, GreenMask, BlueMask, AlphaMask)) {
					output.resize(sizeof(BITMAP) + bmp.info.SizeImage);
					memcpy(output.data(), (char*)&bmp, sizeof(BITMAP));
					memcpy(output.data() + sizeof(BITMAP), Bitmap.get(), bmp.info.SizeImage);
				}
			}
		}
	}

	return Result;
}

bool write2(const pixels& input, buffer& output, unsigned int width, unsigned int height)
{
	BITMAPINFOHEADER bmiHeader = { 0 };
	bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiHeader.biWidth = width;
	bmiHeader.biHeight = height;
	bmiHeader.biCompression = BI_RGB;
	bmiHeader.biClrUsed = 0;
	bmiHeader.biClrImportant = 0;
	bmiHeader.biPlanes = 1;
	bmiHeader.biBitCount = 32;
	bmiHeader.biSizeImage = width * height * 4;
	BITMAPFILEHEADER bmheader = { 0 };
	bmheader.bfType = 'MB';
	bmheader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmheader.bfSize = bmheader.bfOffBits + bmiHeader.biSizeImage;

	output.resize(bmheader.bfSize);
	memcpy(output.data(), &bmheader, sizeof(BITMAPFILEHEADER));
	memcpy(output.data() + sizeof(BITMAPFILEHEADER), &(bmiHeader), sizeof(BITMAPINFOHEADER));
	const rgba* SourcePixel = input.data();
	rgba* TargetPixel = reinterpret_cast<rgba*>(output.data() + bmheader.bfOffBits);
	for (unsigned int y = 0; y < height; ++y)
	{
		for (unsigned int x = 0; x < width; ++x)
		{
			TargetPixel[x + y * width] = SourcePixel[x + (height - 1 - y) * width];
		}
	}
	return true;
}

bool read(const buffer& input, pixels& output, unsigned int& width, unsigned int& height)
{
	BITMAP* bmp = (BITMAP*)input.data();

	if (bmp->head.Signature != BITMAP_SIGNATURE) {
		return false;
	}

	unsigned int ColorTableSize = 0;

	if (bmp->info.BitCount == 1) {
		ColorTableSize = 2;
	}
	else if (bmp->info.BitCount == 4) {
		ColorTableSize = 16;
	}
	else if (bmp->info.BitCount == 8) {
		ColorTableSize = 256;
	}

	BGRA* ColorTable = (BGRA*)(input.data() + sizeof(BITMAP_FILEHEADER) + bmp->info.HeaderSize);

	width = bmp->info.Width < 0 ? -bmp->info.Width : bmp->info.Width;
	height = bmp->info.Height < 0 ? -bmp->info.Height : bmp->info.Height;
	unsigned int bitcount = bmp->info.BitCount;

	output.resize(width * height);

	unsigned int LineWidth = ((width * bitcount / 8) + 3) & ~3;

	uint32_t offset = bmp->head.BitsOffset;

	int Index = 0;
	bool Result = true;

	if (bmp->info.Compression == 0) {
		for (unsigned int i = 0; i < height; i++) {
			const uint8_t* Line = input.data() + offset;
			const uint8_t *LinePtr = Line;
			offset += LineWidth;

			for (unsigned int j = 0; j < width; j++) {
				if (bmp->info.BitCount == 1) {
					uint32_t Color = *((uint8_t*)LinePtr);
					for (int k = 0; k < 8; k++) {
						output[Index].r = ColorTable[Color & 0x80 ? 1 : 0].Red;
						output[Index].g = ColorTable[Color & 0x80 ? 1 : 0].Green;
						output[Index].b = ColorTable[Color & 0x80 ? 1 : 0].Blue;
						output[Index].a = ColorTable[Color & 0x80 ? 1 : 0].Alpha;
						Index++;
						Color <<= 1;
					}
					LinePtr++;
					j += 7;
				}
				else if (bmp->info.BitCount == 4) {
					uint32_t Color = *((uint8_t*)LinePtr);
					output[Index].r = ColorTable[(Color >> 4) & 0x0f].Red;
					output[Index].g = ColorTable[(Color >> 4) & 0x0f].Green;
					output[Index].b = ColorTable[(Color >> 4) & 0x0f].Blue;
					output[Index].a = ColorTable[(Color >> 4) & 0x0f].Alpha;
					Index++;
					output[Index].r = ColorTable[Color & 0x0f].Red;
					output[Index].g = ColorTable[Color & 0x0f].Green;
					output[Index].b = ColorTable[Color & 0x0f].Blue;
					output[Index].a = ColorTable[Color & 0x0f].Alpha;
					Index++;
					LinePtr++;
					j++;
				}
				else if (bmp->info.BitCount == 8) {
					uint32_t Color = *((uint8_t*)LinePtr);
					output[Index].r = ColorTable[Color].Red;
					output[Index].g = ColorTable[Color].Green;
					output[Index].b = ColorTable[Color].Blue;
					output[Index].a = ColorTable[Color].Alpha;
					Index++;
					LinePtr++;
				}
				else if (bmp->info.BitCount == 16) {
					uint32_t Color = *((uint16_t*)LinePtr);
					output[Index].r = ((Color >> 10) & 0x1f) << 3;
					output[Index].g = ((Color >> 5) & 0x1f) << 3;
					output[Index].b = (Color & 0x1f) << 3;
					output[Index].a = 255;
					Index++;
					LinePtr += 2;
				}
				else if (bmp->info.BitCount == 24) {
					uint32_t Color = *((uint32_t*)LinePtr);
					output[Index].b = Color & 0xff;
					output[Index].g = (Color >> 8) & 0xff;
					output[Index].r = (Color >> 16) & 0xff;
					output[Index].a = 255;
					Index++;
					LinePtr += 3;
				}
				else if (bmp->info.BitCount == 32) {
					uint32_t Color = *((uint32_t*)LinePtr);
					output[Index].b = Color & 0xff;
					output[Index].g = (Color >> 8) & 0xff;
					output[Index].r = (Color >> 16) & 0xff;
					output[Index].a = Color >> 24;
					Index++;
					LinePtr += 4;
				}
			}
		}
	}
	else if (bmp->info.Compression == 1) {
		uint8_t Count = 0;
		uint8_t ColorIndex = 0;
		int x = 0, y = 0;

		while (offset < input.size()) {
			Count = input[offset++];
			ColorIndex = input[offset++];

			if (Count > 0) {
				Index = x + y * width;
				for (int k = 0; k < Count; k++) {
					output[Index + k].r = ColorTable[ColorIndex].Red;
					output[Index + k].g = ColorTable[ColorIndex].Green;
					output[Index + k].b = ColorTable[ColorIndex].Blue;
					output[Index + k].a = ColorTable[ColorIndex].Alpha;
				}
				x += Count;
			}
			else if (Count == 0) {
				int Flag = ColorIndex;
				if (Flag == 0) {
					x = 0;
					y++;
				}
				else if (Flag == 1) {
					break;
				}
				else if (Flag == 2) {
					char rx = input[offset++];
					char ry = input[offset++];
					x += rx;
					y += ry;
				}
				else {
					Count = Flag;
					Index = x + y * width;
					for (int k = 0; k < Count; k++) {
						ColorIndex = input[offset++];
						output[Index + k].r = ColorTable[ColorIndex].Red;
						output[Index + k].g = ColorTable[ColorIndex].Green;
						output[Index + k].b = ColorTable[ColorIndex].Blue;
						output[Index + k].a = ColorTable[ColorIndex].Alpha;
					}
					x += Count;
					if (offset & 1) {
						offset++;
					}
				}
			}
		}
	}
	else if (bmp->info.Compression == 2) {
		Result = false;
	}
	else if (bmp->info.Compression == 3) {
		uint32_t BitCountRed = CColor::BitCountByMask(bmp->info.RedMask);
		uint32_t BitCountGreen = CColor::BitCountByMask(bmp->info.GreenMask);
		uint32_t BitCountBlue = CColor::BitCountByMask(bmp->info.BlueMask);
		uint32_t BitCountAlpha = CColor::BitCountByMask(bmp->info.AlphaMask);

		for (unsigned int i = 0; i < height; i++) {
			const uint8_t* Line = input.data() + offset;
			const uint8_t *LinePtr = Line;
			offset += LineWidth;

			for (unsigned int j = 0; j < width; j++) {
				uint32_t Color = 0;
				if (bmp->info.BitCount == 16) {
					Color = *((uint16_t*)LinePtr);
					LinePtr += 2;
				}
				else if (bmp->info.BitCount == 32) {
					Color = *((uint32_t*)LinePtr);
					LinePtr += 4;
				}
				else {
				}
				output[Index].r = CColor::Convert(CColor::ComponentByMask(Color, bmp->info.RedMask), BitCountRed, 8);
				output[Index].g = CColor::Convert(CColor::ComponentByMask(Color, bmp->info.GreenMask), BitCountGreen, 8);
				output[Index].b = CColor::Convert(CColor::ComponentByMask(Color, bmp->info.BlueMask), BitCountBlue, 8);
				output[Index].a = CColor::Convert(CColor::ComponentByMask(Color, bmp->info.AlphaMask), BitCountAlpha, 8);

				Index++;
			}
		}
	}

	return Result;
}

}}
