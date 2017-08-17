#include "Bmp.h"
#include <windows.h>


namespace IMAGE { namespace BMP {

bool Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int /*Quality*/)
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

	TargetBuffer.resize(bmheader.bfSize);

	memcpy(TargetBuffer.data(), &bmheader, sizeof(BITMAPFILEHEADER));
	memcpy(TargetBuffer.data() + sizeof(BITMAPFILEHEADER), &(bmiHeader), sizeof(BITMAPINFOHEADER));

	const uint32_t* SourcePixel = reinterpret_cast<const uint32_t*>(SourceBuffer.data());
	uint32_t* TargetPixel = reinterpret_cast<uint32_t*>(TargetBuffer.data() + bmheader.bfOffBits);
	
	for (int Y = 0; Y < Height; ++Y)
	{
		for (int X = 0; X < Width; ++X)
		{
			TargetPixel[X+Y*Width] = SourcePixel[X+(Height-1-Y)*Width];
		}
	}

	return true;
}

bool Read(const BUFFER& /*SourceBuffer*/, BUFFER& /*TargetBuffer*/, int* /*Width*/, int* /*Height*/)
{
	// todo
	return false;
}

}}
