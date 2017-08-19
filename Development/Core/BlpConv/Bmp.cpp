#include "BlpConv.h"
#include <windows.h>

namespace image { namespace bmp {

bool write(const pixels& input, buffer& output, int Width, int Height, int /*Quality*/)
{
	int Size = Width * Height * 4;
  
	BITMAPINFOHEADER bmiHeader = {0};
	bmiHeader.biSize           = sizeof(BITMAPINFOHEADER);   
	bmiHeader.biWidth          = Width;   
	bmiHeader.biHeight         = Height;   
	bmiHeader.biCompression    = BI_RGB;
	bmiHeader.biClrUsed        = 0;   
	bmiHeader.biClrImportant   = 0;   
	bmiHeader.biPlanes         = 1;   
	bmiHeader.biBitCount       = 32;   
	bmiHeader.biSizeImage      = Size;   

	BITMAPFILEHEADER bmheader = {0}; 
	bmheader.bfType      = 'MB';   
	bmheader.bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER); 
	bmheader.bfSize      = bmheader.bfOffBits+Size;

	output.resize(bmheader.bfSize);

	memcpy(output.data(), &bmheader, sizeof(BITMAPFILEHEADER));
	memcpy(output.data() + sizeof(BITMAPFILEHEADER), &(bmiHeader), sizeof(BITMAPINFOHEADER));

	const rgba* SourcePixel = input.data();
	rgba* TargetPixel = reinterpret_cast<rgba*>(output.data() + bmheader.bfOffBits);
	
	for (int Y = 0; Y < Height; ++Y)
	{
		for (int X = 0; X < Width; ++X)
		{
			TargetPixel[X+Y*Width] = SourcePixel[X+(Height-1-Y)*Width];
		}
	}

	return true;
}

bool read(const buffer& /*input*/, pixels& /*output*/, int* /*Width*/, int* /*Height*/)
{
	// todo
	return false;
}

}}
