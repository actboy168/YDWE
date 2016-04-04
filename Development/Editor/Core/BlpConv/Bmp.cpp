//+-----------------------------------------------------------------------------
//| Included files
//+-----------------------------------------------------------------------------
#include "Bmp.h"
#include <windows.h>

namespace IMAGE 
{

//+-----------------------------------------------------------------------------
//| Constructor
//+-----------------------------------------------------------------------------
BMP::BMP()
{
	//Empty
}


//+-----------------------------------------------------------------------------
//| Destructor
//+-----------------------------------------------------------------------------
BMP::~BMP()
{
	//Empty
}


//+-----------------------------------------------------------------------------
//| Writes BMP data
//+-----------------------------------------------------------------------------
bool BMP::Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int /*Quality*/)
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


	TargetBuffer.Resize(bmheader.bfSize);

	memcpy(TargetBuffer.GetData(), &bmheader, sizeof(BITMAPFILEHEADER));
	memcpy(TargetBuffer.GetData(sizeof(BITMAPFILEHEADER)), &(bmiHeader), sizeof(BITMAPINFOHEADER));
	memcpy(TargetBuffer.GetData(bmheader.bfOffBits), SourceBuffer.GetData(), Size);

	return true;
}


//+-----------------------------------------------------------------------------
//| Reads BMP data
//+-----------------------------------------------------------------------------
bool BMP::Read(const BUFFER& /*SourceBuffer*/, BUFFER& /*TargetBuffer*/, int* /*Width*/, int* /*Height*/)
{
	// todo
	return false;
}

}
