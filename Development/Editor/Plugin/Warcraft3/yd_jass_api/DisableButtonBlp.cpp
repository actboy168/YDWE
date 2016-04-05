#include <base/hook/iat.h>
#include <base/hook/fp_call.h> 	
#include <boost/filesystem.hpp>
#include <base/path/helper.h>	
#include <base/file/stream.h> 
#include <BlpConv/BlpConv.h>
#include <BlpConv/Blp.h>
#include <algorithm>
#include <stdint.h>

namespace base { namespace warcraft3 { namespace japi {

	uint32_t kPasButton[] = {
#include "PasButton.h"
	};

	static inline unsigned char clamp_channel_bits8(int c)
	{
		if (c > 255)
			return 255;
		if (c < 0)
			return 0;
		return static_cast<unsigned char>(c);
	}

	static const size_t kBlpSize = 64;
	bool BlpDisable(const IMAGE::BUFFER& input, IMAGE::BUFFER& output)
	{
		int input_width = 0;
		int input_height = 0;
		IMAGE::BUFFER input_pic, output_pic;
		if (!IMAGE::BLP().Read(input, input_pic, &input_width, &input_height))
		{
			return false;
		}
		if (input_width != kBlpSize || input_height != kBlpSize)
		{
			return false;
		}
		output_pic.Resize(kBlpSize * kBlpSize * 4);
		unsigned char* ptr = output_pic.GetData();
		bool enable = false;
		for (size_t i = 0; i < kBlpSize * kBlpSize; ++i)
		{
			IMAGE::BLP_RGBA const& pixel_a = reinterpret_cast<IMAGE::BLP_RGBA*>(kPasButton)[i];
			IMAGE::BLP_RGBA const& pixel_b = reinterpret_cast<IMAGE::BLP_RGBA*>(input_pic.GetData())[i];
			*ptr = clamp_channel_bits8(((255 - pixel_a.Alpha) * pixel_b.Red + pixel_a.Red) / (255 * 2));
			if (*ptr++ > 63) enable = true;
			*ptr = clamp_channel_bits8(((255 - pixel_a.Alpha) * pixel_b.Green + pixel_a.Green) / (255 * 2));
			if (*ptr++ > 63) enable = true;
			*ptr = clamp_channel_bits8(((255 - pixel_a.Alpha) * pixel_b.Blue + pixel_a.Blue) / (255 * 2));
			if (*ptr++ > 63) enable = true;
			*ptr++ = clamp_channel_bits8(255 - (255 - pixel_a.Alpha) * (255 - pixel_b.Alpha) / 255);
		}
		if (!enable)
		{
			ptr = output_pic.GetData();
			for (size_t i = 0; i < kBlpSize * kBlpSize; ++i)
			{
				*ptr++ *= 2;
				*ptr++ *= 2;
				*ptr++ *= 2;
				ptr++;
			}
		}
		if (!IMAGE::BLP().Write(output_pic, output, kBlpSize, kBlpSize, 95))
		{
			return false;
		}
		return true;
	}

	namespace real
	{
		uintptr_t SMemAlloc = 0;
		uintptr_t SFileLoadFile = 0;
		uintptr_t SFileUnloadFile = 0;
	}

	namespace fake
	{
		void* SMemAlloc(size_t amount)
		{
			return base::std_call<void*>(real::SMemAlloc, amount, ".\\SFile.cpp", 4072, 0);
		}

		bool disable_button_blp(const char* filename, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, OVERLAPPED* overlapped_ptr)
		{
			const void* buffer = 0;
			uint32_t size = 0;
			if (!base::std_call<bool>(real::SFileLoadFile, filename, &buffer, &size, 0, 0))
			{
				return false;
			}
			IMAGE::BUFFER input((const char*)buffer, (const char*)buffer + size);
			IMAGE::BUFFER output;
			base::std_call<bool>(real::SFileUnloadFile, buffer);
			if (!BlpDisable(input, output))
			{
				return false;
			}

			void* result = SMemAlloc(output.GetSize() + reserve_size);
			if (!result)
			{
				return false;
			}
			memcpy(result, output.GetData(), output.GetSize());
			*buffer_ptr = result;
			if (reserve_size) memset((unsigned char*)result + output.GetSize(), 0, reserve_size);
			if (size_ptr) *size_ptr = output.GetSize();
			if (overlapped_ptr && overlapped_ptr->hEvent) ::SetEvent(overlapped_ptr->hEvent);
			return true;
		}

		std::string lastfile;
		bool __stdcall SFileLoadFile(const char* filename, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, OVERLAPPED* overlapped_ptr)
		{
			if (!buffer_ptr || !filename)
			{
				return base::std_call<bool>(real::SFileLoadFile, filename, buffer_ptr, size_ptr, reserve_size, overlapped_ptr);
			}
			if (0 != strnicmp(filename, "replaceabletextures\\commandbuttonsdisabled\\dis", sizeof "replaceabletextures\\commandbuttonsdisabled\\dis" - 1))
			{
				lastfile = filename;
				return base::std_call<bool>(real::SFileLoadFile, filename, buffer_ptr, size_ptr, reserve_size, overlapped_ptr);
			}

			bool suc = base::std_call<bool>(real::SFileLoadFile, filename, buffer_ptr, size_ptr, reserve_size, overlapped_ptr);
			if (suc)
			{
				return true;
			}
			suc = disable_button_blp(lastfile.c_str(), buffer_ptr, size_ptr, reserve_size, overlapped_ptr);
			lastfile = filename;
			return suc;
		}
	}

	void InitializeDisableButtonBlp()
	{
		MessageBox(0, 0, 0, 0);
		HMODULE module_handle = ::GetModuleHandleW(L"Game.dll");
		real::SMemAlloc       = (uintptr_t)::GetProcAddress(::GetModuleHandleW(L"Storm.dll"), (const char*)401);   
		real::SFileUnloadFile = (uintptr_t)::GetProcAddress(::GetModuleHandleW(L"Storm.dll"), (const char*)280); 
		real::SFileLoadFile   = base::hook::iat(module_handle, "Storm.dll", (const char*)(279), (uintptr_t)fake::SFileLoadFile);
	}
}}}
