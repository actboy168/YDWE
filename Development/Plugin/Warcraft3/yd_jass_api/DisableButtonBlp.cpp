#include <base/hook/iat.h>
#include <base/hook/fp_call.h> 	 	  		
#include <base/filesystem.h>
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

	bool BlpBlend(const IMAGE::BUFFER& input_a, const IMAGE::BUFFER& input_b, IMAGE::BUFFER& output)
	{
		int input_width = 0;
		int input_height = 0;
		IMAGE::BUFFER input_a_pic, input_b_pic, output_pic;
		if (!IMAGE::BLP().Read(input_a, input_a_pic, &input_width, &input_height))
		{
			return false;
		}
		if (input_width != kBlpSize || input_height != kBlpSize)
		{
			return false;
		}
		if (!IMAGE::BLP().Read(input_b, input_b_pic, &input_width, &input_height))
		{
			return false;
		}
		if (input_width != kBlpSize || input_height != kBlpSize)
		{
			return false;
		}
		output_pic.Resize(kBlpSize * kBlpSize * 4);
		unsigned char* ptr = output_pic.GetData();
		for (size_t i = 0; i < kBlpSize * kBlpSize; ++i)
		{
			IMAGE::BLP_RGBA const& pixel_a = reinterpret_cast<IMAGE::BLP_RGBA*>(input_a_pic.GetData())[i];
			IMAGE::BLP_RGBA const& pixel_b = reinterpret_cast<IMAGE::BLP_RGBA*>(input_b_pic.GetData())[i];
			*ptr++ = clamp_channel_bits8(((255 - pixel_a.Alpha) * pixel_b.Red + pixel_a.Alpha * pixel_a.Red) / 255);
			*ptr++ = clamp_channel_bits8(((255 - pixel_a.Alpha) * pixel_b.Green + pixel_a.Alpha * pixel_a.Green) / 255);
			*ptr++ = clamp_channel_bits8(((255 - pixel_a.Alpha) * pixel_b.Blue + pixel_a.Alpha * pixel_a.Blue) / 255);
			*ptr++ = clamp_channel_bits8(255 - (255 - pixel_a.Alpha) * (255 - pixel_b.Alpha) / 255);
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
		template <class T>
		struct less : public std::binary_function<T, T, bool>
		{
			bool operator()(const T& lft, const T& rht) const
			{
				return (lft < _Right);
			}
		};

		template <>
		struct less<char> : public std::binary_function<char, char, bool>
		{
			bool operator()(const char& lft, const char& rht) const
			{
				return (tolower(static_cast<unsigned char>(lft)) < tolower(static_cast<unsigned char>(rht)));
			}
		};

		template <>
		struct less<std::string> : public std::binary_function<std::string, std::string, bool>
		{
			bool operator()(const std::string& lft, const std::string& rht) const
			{
				return std::lexicographical_compare(lft.begin(), lft.end(), rht.begin(), rht.end(), less<char>());
			}
		};

		static std::map<std::string, std::string, less<std::string>> g_history;
		static std::map<std::string, IMAGE::BUFFER, less<std::string>> g_virtualblp;
		static std::string g_lastfilepath;

		void* SMemAlloc(size_t amount)
		{
			return base::std_call<void*>(real::SMemAlloc, amount, ".\\SFile.cpp", 4072, 0);
		}

		bool read_virtual_button_blp(const char* filepath, IMAGE::BUFFER& blp)
		{
			auto it = g_virtualblp.find(filepath);
			if (it == g_virtualblp.end())
			{
				return false;
			}
			blp.Assign(it->second.begin(), it->second.end());
			return true;
		}

		bool read_virtual_button_blp(const char* filepath, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, OVERLAPPED* overlapped_ptr)
		{
			auto it = g_virtualblp.find(filepath);
			if (it == g_virtualblp.end())
			{
				return false;
			}
			IMAGE::BUFFER& blp = it->second;
			void* result = SMemAlloc(blp.GetSize() + reserve_size);
			if (!result)
			{
				return false;
			}
			memcpy(result, blp.GetData(), blp.GetSize());
			*buffer_ptr = result;
			if (reserve_size) memset((unsigned char*)result + blp.GetSize(), 0, reserve_size);
			if (size_ptr) *size_ptr = blp.GetSize();
			if (overlapped_ptr && overlapped_ptr->hEvent) ::SetEvent(overlapped_ptr->hEvent);
			return true;
		}

		bool read_button_blp(const char* filepath, IMAGE::BUFFER& blp)
		{
			if (read_virtual_button_blp(filepath, blp))
			{
				return true;
			}
			const void* buffer = 0;
			uint32_t size = 0;
			if (!base::std_call<bool>(real::SFileLoadFile, filepath, &buffer, &size, 0, 0))
			{
				return false;
			}
			blp.Assign((const char*)buffer, (const char*)buffer + size);
			base::std_call<bool>(real::SFileUnloadFile, buffer);
			return true;
		}

		bool disable_button_blp(const char* filename, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size)
		{
			IMAGE::BUFFER input;
			IMAGE::BUFFER output;
			if (!read_button_blp(filename, input))
			{
				return false;
			}
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

		int __stdcall SFileLoadFile(const char* filepath, const void** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, OVERLAPPED* overlapped_ptr)
		{
			if (!buffer_ptr || !filepath)
			{
				return base::std_call<int>(real::SFileLoadFile, filepath, buffer_ptr, size_ptr, reserve_size, overlapped_ptr);
			}
			if (read_virtual_button_blp(filepath, buffer_ptr, size_ptr, reserve_size, overlapped_ptr))
			{
				g_lastfilepath = filepath;
				return 1;
			}

			int suc = base::std_call<int>(real::SFileLoadFile, filepath, buffer_ptr, size_ptr, reserve_size, overlapped_ptr);
			if (suc)
			{
				g_lastfilepath = filepath;
				return suc;
			}
#define DisString "replaceabletextures\\commandbuttonsdisabled\\dis"
#define StrLen(s) (sizeof(s) - 1)
			if (0 == _strnicmp(filepath, DisString, StrLen(DisString)))
			{
				const char* filename = filepath + StrLen(DisString);
				if (0 != _stricmp(ToFileName(g_lastfilepath).c_str(), filename))
				{
					for (const char* cur = filename;; cur += 3)
					{
						auto it = g_history.find(cur);
						if (it != g_history.end())
						{
							suc = disable_button_blp(it->second.c_str(), buffer_ptr, size_ptr, reserve_size);
							break;
						}
						if (0 != _strnicmp(cur, "dis", 3))
						{
							break;
						}
					}
				}
				else
				{
					suc = disable_button_blp(g_lastfilepath.c_str(), buffer_ptr, size_ptr, reserve_size);
					if (suc)
					{
						g_history[filename] = g_lastfilepath;
					}
					else
					{
						if (0 == _strnicmp(g_lastfilepath.c_str(), DisString, StrLen(DisString)))
						{
							for (const char* cur = g_lastfilepath.c_str() + StrLen(DisString);; cur += 3)
							{
								auto it = g_history.find(cur);
								if (it != g_history.end())
								{
									suc = disable_button_blp(it->second.c_str(), buffer_ptr, size_ptr, reserve_size);
									if (suc)
									{
										g_history[filename] = it->second;
									}
									break;
								}
								if (0 != _strnicmp(cur, "dis", 3))
								{
									break;
								}
							}
						}
					}
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

	jass::jboolean_t __cdecl EXBlendButtonIcon(jass::jstring_t input_a, jass::jstring_t intput_b, jass::jstring_t output)
	{
		std::string str_intput_a = jass::from_trigstring(jass::from_string(input_a));
		std::string str_intput_b = jass::from_trigstring(jass::from_string(intput_b));
		std::string str_output = jass::from_trigstring(jass::from_string(output));
		IMAGE::BUFFER buf_input_a, buf_input_b, buf_output;
		if (!fake::read_button_blp(str_intput_a.c_str(), buf_input_a))
		{
			return false;
		}
		if (!fake::read_button_blp(str_intput_b.c_str(), buf_input_b))
		{
			return false;
		}
		if (!BlpBlend(buf_input_a, buf_input_b, fake::g_virtualblp[str_output]))
		{
			fake::g_virtualblp.erase(str_output);
			return false;
		}
		return true;
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
		jass::japi_add((uintptr_t)EXBlendButtonIcon, "EXBlendButtonIcon", "(SSS)B");
		register_game_reset_event([&](uintptr_t)
		{
			fake::g_virtualblp.clear();
			fake::g_lastfilepath.clear();
			fake::g_history.clear();
		});
	}
}}}
