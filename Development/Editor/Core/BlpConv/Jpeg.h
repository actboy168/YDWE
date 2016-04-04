#pragma once

//+-----------------------------------------------------------------------------
//| Prevents stupid redefinitions
//+-----------------------------------------------------------------------------
#define XMD_H


//+-----------------------------------------------------------------------------
//| Included files
//+-----------------------------------------------------------------------------
#include "Buffer.h"


//+-----------------------------------------------------------------------------
//| Included JPEG files (requires C inclusion)
//+-----------------------------------------------------------------------------

#pragma warning(push)
#pragma warning(disable:4005)
extern "C"
{
#include <jpeglib.h>
}
#pragma warning(pop)

namespace IMAGE
{

//+-----------------------------------------------------------------------------
//| Source manager structure
//+-----------------------------------------------------------------------------
struct JPEG_SOURCE_MANAGER
{
	JPEG_SOURCE_MANAGER()
	{
		SourceBuffer = NULL;
		SourceBufferSize = 0;
		Buffer = NULL;
	}

	jpeg_source_mgr Manager;
	unsigned char* SourceBuffer;
	size_t SourceBufferSize;
	JOCTET* Buffer;
};


//+-----------------------------------------------------------------------------
//| Destination manager structure
//+-----------------------------------------------------------------------------
struct JPEG_DESTINATION_MANAGER
{
	JPEG_DESTINATION_MANAGER()
	{
		DestinationBuffer = NULL;
		DestinationBufferSize = 0;
		Buffer = NULL;
	}

	jpeg_destination_mgr Manager;
	unsigned char* DestinationBuffer;
	size_t DestinationBufferSize;
	JOCTET* Buffer;
};


//+-----------------------------------------------------------------------------
//| Jpeg class
//+-----------------------------------------------------------------------------
class JPEG
{
public:
	JPEG();
	~JPEG();

	bool Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality);
	bool Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int* Width = NULL, int* Height = NULL);

protected:
	static void SetMemorySource(jpeg_decompress_struct* Info, unsigned char* Buffer, size_t Size);
	static void SetMemoryDestination(jpeg_compress_struct* Info, unsigned char* Buffer, size_t Size);

	static void SourceInit(jpeg_decompress_struct* Info);
	static boolean SourceFill(jpeg_decompress_struct* Info);
	static void SourceSkip(jpeg_decompress_struct* Info, long NrOfBytes);
	static void SourceTerminate(jpeg_decompress_struct* Info);

	static void DestinationInit(jpeg_compress_struct* Info);
	static boolean DestinationEmpty(jpeg_compress_struct* Info);
	static void DestinationTerminate(jpeg_compress_struct* Info);
};


//+-----------------------------------------------------------------------------
//| Global objects
//+-----------------------------------------------------------------------------
extern JPEG Jpeg;

}
