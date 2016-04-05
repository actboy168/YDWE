//+-----------------------------------------------------------------------------
//| Included files
//+-----------------------------------------------------------------------------
#include "Blp.h"
#include "Jpeg.h" 	
#include <algorithm> 
#include <sstream>  
#include <vector>

namespace IMAGE 
{

//+-----------------------------------------------------------------------------
//| Constructor
//+-----------------------------------------------------------------------------
BLP::BLP()
{
	//Empty
}


//+-----------------------------------------------------------------------------
//| Destructor
//+-----------------------------------------------------------------------------
BLP::~BLP()
{
	//Empty
}


//+-----------------------------------------------------------------------------
//| Writes BLP data
//+-----------------------------------------------------------------------------
bool BLP::Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality)
{
	int32_t i;
	int32_t X;
	int32_t Y;
	int32_t Size;
	int32_t Index;
	int32_t BufferIndex;
	int32_t TotalSize;
	int32_t NrOfMipMaps;
	int32_t TextureSize;
	int32_t CurrentWidth;
	int32_t CurrentHeight;
	int32_t CurrentOffset;
	BUFFER TempBuffer;
	BLP_HEADER Header;
	const unsigned char* Pointer;
	uint32_t JpegHeaderSize;
	std::stringstream Stream;
	std::vector<BUFFER> MipMapBufferList;

	JpegHeaderSize = 4;
	MipMapBufferList.resize(MAX_NR_OF_BLP_MIP_MAPS);

	Header.Compression = 0;
	Header.Flags = 8;
	Header.Width = Width;
	Header.Height = Height;
	Header.PictureType = 4;
	Header.PictureSubType = 1;

	NrOfMipMaps = 0;

	Size = std::max(Header.Width, Header.Height);
	while (Size >= 1)
	{
		Size /= 2;
		NrOfMipMaps++;
	}

	if (NrOfMipMaps > MAX_NR_OF_BLP_MIP_MAPS)
	{
		NrOfMipMaps = MAX_NR_OF_BLP_MIP_MAPS;
	}

	if (NrOfMipMaps < 1)
	{
		return FALSE;
	}

	CurrentWidth = Header.Width;
	CurrentHeight = Header.Height;
	CurrentOffset = sizeof(BLP_HEADER) + sizeof(uint32_t) + JpegHeaderSize;
	for (i = 0; i < NrOfMipMaps; i++)
	{
		TempBuffer.Resize(CurrentWidth * CurrentHeight * 4);

		Index = 0;
		BufferIndex = 0;
		Pointer = reinterpret_cast<const unsigned char*>(SourceBuffer.GetData());

		for (Y = 0; Y < static_cast<int32_t>(CurrentHeight); Y++)
		{
			for (X = 0; X < static_cast<int32_t>(CurrentWidth); X++)
			{
				TempBuffer[BufferIndex++] = Pointer[Index++];
				TempBuffer[BufferIndex++] = Pointer[Index++];
				TempBuffer[BufferIndex++] = Pointer[Index++];
				TempBuffer[BufferIndex++] = Pointer[Index++];
			}
		}

		if (!Jpeg.Write(TempBuffer, MipMapBufferList[i], CurrentWidth, CurrentHeight, Quality))
		{
			return FALSE;
		}

		TextureSize = MipMapBufferList[i].GetSize();

		Header.Offset[i] = CurrentOffset;
		Header.Size[i] = TextureSize - JpegHeaderSize;

		CurrentWidth /= 2;
		CurrentHeight /= 2;
		CurrentOffset += Header.Size[i];

		if (CurrentWidth < 1) CurrentWidth = 1;
		if (CurrentHeight < 1) CurrentHeight = 1;
	}

	TotalSize = sizeof(BLP_HEADER) + sizeof(uint32_t) + JpegHeaderSize;
	for (i = 0; i < NrOfMipMaps; i++)
	{
		if (MipMapBufferList[i].GetSize() <= 0) break;
		TotalSize += Header.Size[i];
	}

	TargetBuffer.Resize(TotalSize);

	CurrentOffset = 0;

	memcpy(&TargetBuffer[CurrentOffset], &Header, sizeof(BLP_HEADER));
	CurrentOffset += sizeof(BLP_HEADER);

	memcpy(&TargetBuffer[CurrentOffset], &JpegHeaderSize, sizeof(uint32_t));
	CurrentOffset += sizeof(uint32_t);

	Size = Header.Size[0] + JpegHeaderSize;
	memcpy(&TargetBuffer[CurrentOffset], &((MipMapBufferList[0])[0]), Size);
	CurrentOffset += Size;

	for (i = 1; i < NrOfMipMaps; i++)
	{
		if (MipMapBufferList[i].GetSize() <= 0) break;

		memcpy(&TargetBuffer[CurrentOffset], &((MipMapBufferList[i])[JpegHeaderSize]), Header.Size[i]);
		CurrentOffset += Header.Size[i];
	}
	return true;
}


//+-----------------------------------------------------------------------------
//| Reads BLP data
//+-----------------------------------------------------------------------------
bool BLP::Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int* Width, int* Height)
{
	BLP_HEADER Header;

	memcpy(reinterpret_cast<char*>(&Header), SourceBuffer.GetData(), sizeof(BLP_HEADER));
	if (Header.MagicNumber != '1PLB')
	{
		LOG("The file is not a BLP file!");
		return false;
	}

	switch(Header.Compression)
	{
	case 0:
		{
			if(!LoadCompressed(Header, SourceBuffer, TargetBuffer)) return false;
			break;
		}
	case 1:
		{
			if(!LoadUncompressed(Header, SourceBuffer, TargetBuffer)) return false;
			break;
		}
	default:
		{
			LOG("Unable to load  blp file, unknown compression method!");
			return false;
		}
	}

	if(Width != NULL) (*Width) = Header.Width;
	if(Height != NULL) (*Height) = Header.Height;

	return true;
}


//+-----------------------------------------------------------------------------
//| Loads a compressed blp
//+-----------------------------------------------------------------------------
bool BLP::LoadCompressed(BLP_HEADER& Header, const BUFFER& SourceBuffer, BUFFER& TargetBuffer)
{
	BUFFER TempBuffer;
	uint32_t  JpegHeaderSize;

	memcpy(reinterpret_cast<char*>(&JpegHeaderSize), SourceBuffer.GetData(sizeof(BLP_HEADER)), sizeof(uint32_t));

	TempBuffer.Resize(Header.Size[0] + JpegHeaderSize);

	memcpy(TempBuffer.GetData(0), SourceBuffer.GetData(sizeof(BLP_HEADER) + sizeof(uint32_t)), JpegHeaderSize);
	memcpy(TempBuffer.GetData(JpegHeaderSize), SourceBuffer.GetData(Header.Offset[0]), Header.Size[0]);

	int Width;
	int Height;
	if (!Jpeg.Read(TempBuffer, TargetBuffer, &Width, &Height))
	{
		LOG("Unable to load  blp file, BLP reading failed!");
		return false;
	}

	return true;
}


//+-----------------------------------------------------------------------------
//| Loads an uncompressed blp
//+-----------------------------------------------------------------------------
bool BLP::LoadUncompressed(BLP_HEADER& Header, const BUFFER& SourceBuffer, BUFFER& TargetBuffer)
{
	static const int PALETTE_SIZE = 256;
	BLP_RGBA const* Palette = reinterpret_cast<BLP_RGBA const*>(SourceBuffer.GetData(sizeof(BLP_HEADER)));
	BLP_PIXEL const* SourcePixel = reinterpret_cast<BLP_PIXEL const*>(SourceBuffer.GetData(Header.Offset[0]));
	int Size = Header.Width * Header.Height;
	TargetBuffer.Resize(Size * 4);
	BLP_RGBA* TargetPixel = reinterpret_cast<BLP_RGBA*>(TargetBuffer.GetData());

	switch(Header.PictureType)
	{
	case 3:
	case 4:
		for (int i = 0; i < Size; i++)
		{
			TargetPixel[i] = Palette[SourcePixel[i].Index];
		}
		for (int i = 0; i < Size; i++)
		{
			TargetPixel[i].Alpha = SourcePixel[Size + i].Index;
		}
		break;
	case 5:
		for (int i = 0; i < Size; i++)
		{
			TargetPixel[i] = Palette[SourcePixel[i].Index];
			TargetPixel[i].Alpha = 255 - TargetPixel[i].Alpha;
		}
		break;
	default:
		LOG("Unable to load  blp file, unknown picture type!");
		return false;
	}
	return true;
}

}
