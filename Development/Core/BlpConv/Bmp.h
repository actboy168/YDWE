#pragma once

//+-----------------------------------------------------------------------------
//| Included files
//+-----------------------------------------------------------------------------
#include "Buffer.h"


namespace IMAGE 
{

//+-----------------------------------------------------------------------------
//| Bmp class
//+-----------------------------------------------------------------------------
class BMP
{
public:
	BMP();
	~BMP();

	bool Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality);
	bool Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int* Width = NULL, int* Height = NULL);

protected:
};

}
