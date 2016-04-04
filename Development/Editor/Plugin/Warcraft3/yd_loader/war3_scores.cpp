#include <base/warcraft3/jass/hook.h> 	
#include <base/warcraft3/jass.h>
#include <base/warcraft3/event.h>
#include <base/hook/fp_call.h>
#include <stdint.h>	
#include <map>
#include <array>
#include <string>
#include <Windows.h>

namespace scores {
	using namespace base::warcraft3;
	namespace rpg {
		jass::jhandle_t g_gc = 0;
		uint16_t g_dict = 0;

		namespace real {
			uintptr_t InitGameCache = 0;
			uintptr_t StoreInteger = 0;
			uintptr_t GetStoredInteger = 0;
		}

		namespace cache {
			std::array<std::map<std::string, int32_t>, 12> cache_map;
			std::array<std::map<std::string, int32_t>, 12> current_map;
			bool set(const char* table, const char* key, int32_t value)
			{
				if (!table || !key)
				{
					return false;
				}
				if ((table[0] < 'A') || (table[0] > 'A' + 11))
				{
					return true;
				}
				int playerid = table[0] - 'A';
				if (table[1] == '=')
				{
					cache_map[playerid][key] = value;
					return true;
				}
				else if (table[1] == '+')
				{
					std::string szkey = key;
					size_t npos = szkey.find("+");
					if (npos != -1)
					{
						try {
							int32_t max = (int32_t)std::stol(szkey.substr(npos + 1, std::string::npos));
							int32_t current_value = current_map[playerid][key];
							if (max < value + current_value)
							{
								return false;
							}
							cache_map[playerid][key] = cache_map[playerid][key] + value;
							current_map[playerid][key] = current_value + value;
							return true;
						}
						catch (...) {
							return false;
						}
					}
				}
				return true;
			}

			int32_t get(const char* table, const char* key)
			{
				if (!table || !key || (table[0] < 'A') || (table[0] > 'A' + 11))
				{
					return 0;
				}
				int playerid = table[0] - 'A';
				return cache_map[playerid][key];
			}
		}

		namespace fake {
			jass::jhandle_t __cdecl InitGameCache(jass::jstring_t name)
			{
				const char* gcname = jass::from_string(name);

				if (gcname[2] == '.' && gcname[3] == 'x' && gcname[4] == '\0')
				{
					g_gc = base::c_call<jass::jhandle_t>(real::InitGameCache, name);
					g_dict = gcname[1] | (gcname[0] << 8);
					return g_gc;
				}
				else
				{
					return base::c_call<jass::jhandle_t>(real::InitGameCache, name);
				}
			}
			void __cdecl StoreInteger(jass::jhandle_t gc, jass::jstring_t table, jass::jstring_t key, jass::jinteger_t value)
			{
				if (g_gc == gc)
				{
					const char* sztable = jass::from_string(table);
					const char* szkey = jass::from_string(key);
					if (cache::set(sztable, szkey, value))
					{
					}
				}
				base::c_call<void>(real::StoreInteger, gc, table, key, value);
			}
			jass::jinteger_t __cdecl GetStoredInteger(jass::jhandle_t gc, jass::jstring_t table, jass::jstring_t key)
			{
				if (g_gc == gc)
				{
					return cache::get(jass::from_string(table), jass::from_string(key));
				}
				return base::c_call<jass::jinteger_t>(real::GetStoredInteger, gc, table, key);
			}
		}

		void hook()
		{
			jass::async_hook("InitGameCache", (uintptr_t*)&real::InitGameCache, (uintptr_t)fake::InitGameCache);
			jass::async_hook("StoreInteger", (uintptr_t*)&real::StoreInteger, (uintptr_t)fake::StoreInteger);
			jass::async_hook("GetStoredInteger", (uintptr_t*)&real::GetStoredInteger, (uintptr_t)fake::GetStoredInteger);
			register_game_reset_event([&](uintptr_t)
			{
				g_gc = 0;
				g_dict = 0;
				for (size_t i = 0; i < 12; ++i)
				{
					cache::cache_map[i].clear();
					cache::current_map[i].clear();
				}
			});
		}

		void unhook()
		{
			jass::async_unhook("InitGameCache", (uintptr_t*)&real::InitGameCache, (uintptr_t)fake::InitGameCache);
			jass::async_unhook("StoreInteger", (uintptr_t*)&real::StoreInteger, (uintptr_t)fake::StoreInteger);
			jass::async_unhook("GetStoredInteger", (uintptr_t*)&real::GetStoredInteger, (uintptr_t)fake::GetStoredInteger);
		}
	}
}
