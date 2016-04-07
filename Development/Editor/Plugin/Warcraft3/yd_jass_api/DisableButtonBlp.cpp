#include <base/hook/iat.h>
#include <base/hook/fp_call.h> 	
#include <boost/filesystem.hpp>
#include <base/path/helper.h>	
#include <base/file/stream.h> 
#include <base/warcraft3/virtual_mpq.h>  
#include <base/warcraft3/jass/hook.h>	
#include <base/warcraft3/jass.h>	
#include <base/warcraft3/event.h>
#include <BlpConv/BlpConv.h>
#include <BlpConv/Blp.h>
#include <algorithm>
#include <map>
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

	std::string ToFileName(const std::string& file)
	{
		size_t pos = file.find_last_of('\\');
		if (pos == std::string::npos)
		{
			return file;
		}
		return file.substr(pos + 1, std::string::npos);
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

		bool disable_button_blp(const char* filename, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size)
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
			return true;
		}

		static std::map<std::string, std::string> g_history;
		static std::string g_lastfilepath;
		bool __stdcall SFileLoadFile(const char* filepath, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, OVERLAPPED* overlapped_ptr)
		{
			if (!buffer_ptr || !filepath)
			{
				return base::std_call<bool>(real::SFileLoadFile, filepath, buffer_ptr, size_ptr, reserve_size, overlapped_ptr);
			}
			bool suc = base::std_call<bool>(real::SFileLoadFile, filepath, buffer_ptr, size_ptr, reserve_size, overlapped_ptr);
			if (suc)
			{
				g_lastfilepath = filepath;
				return true;
			}
#define DisString "replaceabletextures\\commandbuttonsdisabled\\dis"
#define StrLen(s) (sizeof(s) - 1)
			if (0 == strnicmp(filepath, DisString, StrLen(DisString)))
			{
				const char* filename = filepath + StrLen(DisString);
				if (0 != stricmp(ToFileName(g_lastfilepath).c_str(), filename))
				{
					for (const char* cur = filename;; cur += 3)
					{
						auto it = g_history.find(cur);
						if (it != g_history.end())
						{
							suc = disable_button_blp(it->second.c_str(), buffer_ptr, size_ptr, reserve_size);
							break;
						}
						if (0 != strnicmp(cur, "dis", 3))
						{
							break;
						}
					}
				}
				else
				{
					g_history[filename] = g_lastfilepath;
					suc = disable_button_blp(g_lastfilepath.c_str(), buffer_ptr, size_ptr, reserve_size);
				}
			}
			g_lastfilepath = filepath;
			if (overlapped_ptr && overlapped_ptr->hEvent) ::SetEvent(overlapped_ptr->hEvent);
			return suc;
		}
	}

	void __cdecl EXDclareButtonIcon(jass::jstring_t path)
	{
		std::string str_path = jass::from_trigstring(jass::from_string(path));
		fake::g_history[ToFileName(str_path)] = str_path;
	}

	void InitializeDisableButtonBlp()
	{
		// 会和虚拟mpq冲突，暂时的解决方法先把虚拟mpq加载起来
		HMODULE module_handle = ::GetModuleHandleW(L"Game.dll");
		virtual_mpq::initialize(module_handle);
		real::SMemAlloc       = (uintptr_t)::GetProcAddress(::GetModuleHandleW(L"Storm.dll"), (const char*)401);   
		real::SFileUnloadFile = (uintptr_t)::GetProcAddress(::GetModuleHandleW(L"Storm.dll"), (const char*)280); 
		real::SFileLoadFile = base::hook::iat(module_handle, "Storm.dll", (const char*)(279), (uintptr_t)fake::SFileLoadFile);
		jass::japi_add((uintptr_t)EXDclareButtonIcon, "EXDclareButtonIcon", "(S)V");
		register_game_reset_event([&](uintptr_t)
		{
			fake::g_lastfilepath.clear();
			fake::g_history.clear();
		});
	}
}}}
