#include "Jpeg.h"
#include <algorithm>

#pragma warning(push)
#pragma warning(disable:4005)
extern "C"
{
#include <jpeglib.h>
}
#pragma warning(pop)

namespace IMAGE { namespace JPEG {

struct JPEG_SOURCE_MANAGER
{
	JPEG_SOURCE_MANAGER()
	{
		SourceBuffer = NULL;
		SourceBufferSize = 0;
		Buffer = NULL;
	}

	jpeg_source_mgr Manager;
	const JOCTET* SourceBuffer;
	size_t SourceBufferSize;
	const JOCTET* Buffer;
};

struct JPEG_DESTINATION_MANAGER
{
	JPEG_DESTINATION_MANAGER()
	{
		DestinationBuffer = NULL;
		DestinationBufferSize = 0;
		Buffer = NULL;
	}

	jpeg_destination_mgr Manager;
	JOCTET* DestinationBuffer;
	size_t DestinationBufferSize;
	JOCTET* Buffer;
};

static void SourceInit(jpeg_decompress_struct* /*Info*/)
{
	//Empty
}

static boolean SourceFill(jpeg_decompress_struct* Info)
{
	JPEG_SOURCE_MANAGER* SourceManager;

	SourceManager = reinterpret_cast<JPEG_SOURCE_MANAGER*>(Info->src);

	SourceManager->Buffer = SourceManager->SourceBuffer;
	SourceManager->Manager.next_input_byte = SourceManager->Buffer;
	SourceManager->Manager.bytes_in_buffer = SourceManager->SourceBufferSize;

	return true;
}

static void SourceSkip(jpeg_decompress_struct* Info, long NrOfBytes)
{
	JPEG_SOURCE_MANAGER* SourceManager;

	SourceManager = reinterpret_cast<JPEG_SOURCE_MANAGER*>(Info->src);

	if(NrOfBytes > 0)
	{
		while(NrOfBytes > static_cast<long>(SourceManager->Manager.bytes_in_buffer))
		{
			NrOfBytes -= static_cast<long>(SourceManager->Manager.bytes_in_buffer);
			SourceFill(Info);
		}

		SourceManager->Manager.next_input_byte += NrOfBytes;
		SourceManager->Manager.bytes_in_buffer -= NrOfBytes;
	}
}

static void SourceTerminate(jpeg_decompress_struct* /*Info*/)
{
	//Empty
}

static void DestinationInit(jpeg_compress_struct* Info)
{
	JPEG_DESTINATION_MANAGER* DestinationManager;

	DestinationManager = reinterpret_cast<JPEG_DESTINATION_MANAGER*>(Info->dest);

	DestinationManager->Buffer = DestinationManager->DestinationBuffer;
	DestinationManager->Manager.next_output_byte = DestinationManager->Buffer;
	DestinationManager->Manager.free_in_buffer = DestinationManager->DestinationBufferSize;
}

static boolean DestinationEmpty(jpeg_compress_struct* Info)
{
	JPEG_DESTINATION_MANAGER* DestinationManager;

	DestinationManager = reinterpret_cast<JPEG_DESTINATION_MANAGER*>(Info->dest);

	DestinationManager->Manager.next_output_byte = DestinationManager->Buffer;
	DestinationManager->Manager.free_in_buffer = DestinationManager->DestinationBufferSize;

	return true;
}

static void DestinationTerminate(jpeg_compress_struct* /*Info*/)
{
	//Empty
}

static void SetMemorySource(jpeg_decompress_struct* Info, const JOCTET* Buffer, size_t Size)
{
	JPEG_SOURCE_MANAGER* SourceManager;

	Info->src = reinterpret_cast<jpeg_source_mgr*>((*Info->mem->alloc_small)(reinterpret_cast<j_common_ptr>(Info), JPOOL_PERMANENT, sizeof(JPEG_SOURCE_MANAGER)));
	SourceManager = reinterpret_cast<JPEG_SOURCE_MANAGER*>(Info->src);

	SourceManager->Buffer = reinterpret_cast<JOCTET*>((*Info->mem->alloc_small)(reinterpret_cast<j_common_ptr>(Info), JPOOL_PERMANENT, Size * sizeof(JOCTET)));
	SourceManager->SourceBuffer = Buffer;
	SourceManager->SourceBufferSize = Size;
	SourceManager->Manager.init_source = SourceInit;
	SourceManager->Manager.fill_input_buffer = SourceFill;
	SourceManager->Manager.skip_input_data = SourceSkip;
	SourceManager->Manager.resync_to_restart = jpeg_resync_to_restart;
	SourceManager->Manager.term_source = SourceTerminate;
	SourceManager->Manager.bytes_in_buffer = 0;
	SourceManager->Manager.next_input_byte = NULL;
}

static void SetMemoryDestination(jpeg_compress_struct* Info, JOCTET* Buffer, size_t Size)
{
	JPEG_DESTINATION_MANAGER* DestinationManager;

	Info->dest = reinterpret_cast<jpeg_destination_mgr*>((*Info->mem->alloc_small)(reinterpret_cast<j_common_ptr>(Info), JPOOL_PERMANENT, sizeof(JPEG_DESTINATION_MANAGER)));
	DestinationManager = reinterpret_cast<JPEG_DESTINATION_MANAGER*>(Info->dest);

	DestinationManager->Buffer = NULL;
	DestinationManager->DestinationBuffer = Buffer;
	DestinationManager->DestinationBufferSize = Size;
	DestinationManager->Manager.init_destination = DestinationInit;
	DestinationManager->Manager.empty_output_buffer = DestinationEmpty;
	DestinationManager->Manager.term_destination = DestinationTerminate;
}

bool Write(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, int Width, int Height, int Quality)
{
	int Stride;
	int RealSize;
	int DummySize;
	BUFFER TempBuffer;
	JSAMPROW Pointer[1];
	jpeg_compress_struct Info;
	jpeg_error_mgr ErrorManager;

	Info.err = jpeg_std_error(&ErrorManager);

	DummySize = ((Width * Height * 4) * 2) + 10000;
	TempBuffer.resize(DummySize);

	jpeg_create_compress(&Info);

	SetMemoryDestination(&Info, TempBuffer.data(), TempBuffer.size());

	Info.image_width = Width;
	Info.image_height = Height;
	Info.input_components = 4;
	Info.in_color_space = JCS_UNKNOWN;

	jpeg_set_defaults(&Info);
	jpeg_set_quality(&Info, Quality, true);
	jpeg_start_compress(&Info, true);

	Stride = Width * 4;
	while (Info.next_scanline < Info.image_height)
	{
		Pointer[0] = (JSAMPROW)(SourceBuffer.data() + Info.next_scanline * Stride);
		jpeg_write_scanlines(&Info, Pointer, 1);
	}

	jpeg_finish_compress(&Info);

	RealSize = DummySize - static_cast<int>(Info.dest->free_in_buffer);
	TargetBuffer.resize(RealSize);

	memcpy(TargetBuffer.data(), TempBuffer.data(), RealSize);

	jpeg_destroy_compress(&Info);

	return true;
}

bool Read(const BUFFER& SourceBuffer, BUFFER& TargetBuffer, unsigned int Width, unsigned int Height)
{
	jpeg_decompress_struct Info;
	jpeg_error_mgr ErrorManager;
	Info.err = jpeg_std_error(&ErrorManager);

	jpeg_create_decompress(&Info);
	SetMemorySource(&Info, SourceBuffer.data(), SourceBuffer.size());
	jpeg_read_header(&Info, true);
	jpeg_start_decompress(&Info);

	if ((Info.output_components != 3) && (Info.output_components != 4))
	{
		LOG("Nr of channels must be 3 or 4!");
		return false;
	}

	TargetBuffer.resize(Width * Height * 4);
	JSAMPARRAY buffer = (*Info.mem->alloc_sarray)(reinterpret_cast<j_common_ptr>(&Info), JPOOL_IMAGE, Info.output_width * Info.output_components, 1);

	struct rgba_t
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	rgba_t* output = reinterpret_cast<rgba_t*>(TargetBuffer.data());
	unsigned int minw = (std::min)(Width, Info.output_width);
	for (unsigned int y = 0; y < Height; ++y)
	{
		if (Info.output_scanline < Info.output_height)
		{
			jpeg_read_scanlines(&Info, buffer, 1);

			if (Info.output_components == 3)
			{
				for (unsigned int x = 0; x < minw; ++x)
				{
					output[0].r = buffer[0][3 * x + 0];
					output[0].g = buffer[0][3 * x + 1];
					output[0].b = buffer[0][3 * x + 2];
					output[0].a = 255;
					output++;
				}
			}
			else
			{
				memcpy(output, buffer[0], minw * sizeof rgba_t);
				output += minw;
			}

			if (minw < Width)
			{
				memset(output, 0, (Width - minw) * sizeof rgba_t);
				output += (Width - minw);
			}
		}
		else
		{
			memset(output, 0, Width * sizeof rgba_t);
			output += Width;
		}

	}

	jpeg_destroy_decompress(&Info);

	return true;
}

}}
