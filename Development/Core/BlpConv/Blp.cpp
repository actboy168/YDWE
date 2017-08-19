#include "BlpConv.h"
#include <algorithm> 
#include <sstream>  
#include <vector>

#define LOG(s)

namespace image { namespace blp {

const int MAX_NR_OF_BLP_MIP_MAPS = 16;

struct BLP_HEADER
{
	BLP_HEADER()
	{
		MagicNumber = '1PLB';
		Compression = 0;
		AlphaBits = 0;
		Width = 0;
		Height = 0;
		Unknown1 = 0;
		Unknown2 = 0;
		memset(Offset, 0, MAX_NR_OF_BLP_MIP_MAPS * sizeof(uint32_t));
		memset(Size, 0, MAX_NR_OF_BLP_MIP_MAPS * sizeof(uint32_t));
	}

	uint32_t MagicNumber;
	uint32_t Compression;
	uint32_t AlphaBits;
	uint32_t Width;
	uint32_t Height;
	uint32_t Unknown1;
	uint32_t Unknown2;
	uint32_t Offset[MAX_NR_OF_BLP_MIP_MAPS];
	uint32_t Size[MAX_NR_OF_BLP_MIP_MAPS];
};

static bool LoadCompressed(BLP_HEADER& Header, const buffer& input, pixels& output)
{
	buffer temp;
	uint32_t  JpegHeaderSize;
	memcpy(reinterpret_cast<char*>(&JpegHeaderSize), input.data() + sizeof(BLP_HEADER), sizeof(uint32_t));
	temp.resize(Header.Size[0] + JpegHeaderSize);
	memcpy(temp.data(), input.data() + sizeof(BLP_HEADER) + sizeof(uint32_t), JpegHeaderSize);
	memcpy(temp.data() + JpegHeaderSize, input.data() + Header.Offset[0], Header.Size[0]);
	return jpeg::read(temp, output, Header.Width, Header.Height);
}

static bool LoadUncompressed(BLP_HEADER& Header, const buffer& input, pixels& output)
{
	static const int PALETTE_SIZE = 256;
	rgba const* Palette = reinterpret_cast<rgba const*>(input.data() + sizeof(BLP_HEADER));
	uint8_t const* SourcePixel = reinterpret_cast<uint8_t const*>(input.data() + Header.Offset[0]);
	int Size = Header.Width * Header.Height;
	output.resize(Size);
	rgba* TargetPixel = output.data();
	uint8_t const* SourceAlpha = SourcePixel + Size;
	switch (Header.AlphaBits)
	{
	default:
	case 0:
		for (int i = 0; i < Size; i++)
		{
			TargetPixel[i] = Palette[SourcePixel[i]];
			TargetPixel[i].a = 255;
		}
		break;
	case 1:
		for (int i = 0; i < Size; i++)
		{
			TargetPixel[i] = Palette[SourcePixel[i]];
			TargetPixel[i].a = (SourceAlpha[i >> 3] & (1 << (i & 7))) ? 1 : 0;
		}
		break;
	case 4:
		for (int i = 0; i < Size; i++)
		{
			TargetPixel[i] = Palette[SourcePixel[i]];
			switch (i & 1)
			{
			case 0: TargetPixel[i].a = SourceAlpha[i >> 1] & 0x0F; break;
			case 1: TargetPixel[i].a = (SourceAlpha[i >> 1] & 0xF0) >> 4; break;
			}
		}
		break;
	case 8:
		for (int i = 0; i < Size; i++)
		{
			TargetPixel[i] = Palette[SourcePixel[i]];
			TargetPixel[i].a = SourceAlpha[i];
		}
		break;
	}
	return true;
}

bool write(const pixels& input, buffer& output, int Width, int Height, int Quality)
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
	pixels temp;
	BLP_HEADER Header;
	uint32_t JpegHeaderSize;
	std::stringstream Stream;
	std::vector<buffer> MipMapBufferList;

	JpegHeaderSize = 4;
	MipMapBufferList.resize(MAX_NR_OF_BLP_MIP_MAPS);

	Header.Compression = 0;
	Header.AlphaBits = 8;
	Header.Width = Width;
	Header.Height = Height;
	Header.Unknown1 = 4;
	Header.Unknown2 = 1;

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
		return false;
	}

	CurrentWidth = Header.Width;
	CurrentHeight = Header.Height;
	CurrentOffset = sizeof(BLP_HEADER) + sizeof(uint32_t) + JpegHeaderSize;
	for (i = 0; i < NrOfMipMaps; i++)
	{
		temp.resize(CurrentWidth * CurrentHeight);

		Index = 0;
		BufferIndex = 0;

		for (Y = 0; Y < static_cast<int32_t>(CurrentHeight); Y++)
		{
			for (X = 0; X < static_cast<int32_t>(CurrentWidth); X++)
			{
				temp[BufferIndex++] = input[Index++];
			}
		}

		if (!jpeg::write(temp, MipMapBufferList[i], CurrentWidth, CurrentHeight, Quality))
		{
			return false;
		}

		TextureSize = MipMapBufferList[i].size();

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
		if (MipMapBufferList[i].size() <= 0) break;
		TotalSize += Header.Size[i];
	}

	output.resize(TotalSize);

	CurrentOffset = 0;

	memcpy(&output[CurrentOffset], &Header, sizeof(BLP_HEADER));
	CurrentOffset += sizeof(BLP_HEADER);

	memcpy(&output[CurrentOffset], &JpegHeaderSize, sizeof(uint32_t));
	CurrentOffset += sizeof(uint32_t);

	Size = Header.Size[0] + JpegHeaderSize;
	memcpy(&output[CurrentOffset], &((MipMapBufferList[0])[0]), Size);
	CurrentOffset += Size;

	for (i = 1; i < NrOfMipMaps; i++)
	{
		if (MipMapBufferList[i].size() <= 0) break;

		memcpy(&output[CurrentOffset], &((MipMapBufferList[i])[JpegHeaderSize]), Header.Size[i]);
		CurrentOffset += Header.Size[i];
	}
	return true;
}

bool read(const buffer& input, pixels& output, unsigned int* Width, unsigned int* Height)
{
	BLP_HEADER Header;

	memcpy(reinterpret_cast<char*>(&Header), input.data(), sizeof(BLP_HEADER));
	if (Header.MagicNumber != '1PLB')
	{
		LOG("The file is not a BLP file!");
		return false;
	}

	switch (Header.Compression)
	{
	default:
	case 0:
		if (!LoadCompressed(Header, input, output)) return false;
		break;
	case 1:
		if (!LoadUncompressed(Header, input, output)) return false;
		break;
	}

	if (Width != NULL) (*Width) = Header.Width;
	if (Height != NULL) (*Height) = Header.Height;

	return true;
}

}}
