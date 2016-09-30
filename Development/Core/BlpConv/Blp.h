#pragma once

//+-----------------------------------------------------------------------------
//| Included files
//+-----------------------------------------------------------------------------
#include "Buffer.h"


namespace IMAGE 
{

//+-----------------------------------------------------------------------------
//| Constants
//+-----------------------------------------------------------------------------
const int MAX_NR_OF_BLP_MIP_MAPS = 16;


//+-----------------------------------------------------------------------------
//| Blp header structure
//+-----------------------------------------------------------------------------
struct BLP_HEADER
{
	BLP_HEADER()
	{
		MagicNumber= '1PLB';
		Compression = 0;
		Flags = 0;
		Width = 0;
		Height = 0;
		PictureType = 0;
		PictureSubType = 0;
		memset(Offset, 0, MAX_NR_OF_BLP_MIP_MAPS * sizeof(uint32_t));
		memset(Size, 0, MAX_NR_OF_BLP_MIP_MAPS * sizeof(uint32_t));
	}

	uint32_t MagicNumber;
	uint32_t Compression;
	uint32_t Flags;
	uint32_t Width;
	uint32_t Height;
	uint32_t PictureType;
	uint32_t PictureSubType;
	uint32_t Offset[MAX_NR_OF_BLP_MIP_MAPS];
	uint32_t Size[MAX_NR_OF_BLP_MIP_MAPS];
};


//+-----------------------------------------------------------------------------
//| Blp RGBA structure
//+-----------------------------------------------------------------------------
struct BLP_RGBA
{
	uint8_t Red;
	uint8_t Green;
	uint8_t Blue;
	uint8_t Alpha;
};

//+-----------------------------------------------------------------------------
//| Blp pixel structure
//+-----------------------------------------------------------------------------
struct BLP_PIXEL
{
	uint8_t Index;
};

//+-----------------------------------------------------------------------------
//| Blp class
//+-----------------------------------------------------------------------------
class BLP
{
public:
	BLP();
	~BLP();

	bool Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality);
	bool Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int* Width = NULL, int* Height = NULL);

protected:
	bool LoadCompressed(BLP_HEADER& Header, const BUFFER& SourceBuffer, BUFFER& TargetBuffer);
	bool LoadUncompressed(BLP_HEADER& Header, const BUFFER& SourceBuffer, BUFFER& TargetBuffer);
};

}
