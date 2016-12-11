#include <base/hook/fp_call.h>
#include <base/hook/iat.h>
#include <base/warcraft3/jass/hook.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/version.h>
#include <base/warcraft3/war3_searcher.h>	
#include <base/warcraft3/hashtable.h>
#include <base/hook/inline.h>
#include <array>
#include <string>
#include "StringPool.h"

namespace base { namespace warcraft3 { namespace japi {
	extern string_pool_t string_pool;

	enum ITEM_DATA_TYPE
	{
		ITEM_DATA_ART = 1,
		ITEM_DATA_TIP,
		ITEM_DATA_UBERTIP,
		ITEM_DATA_NAME,
		ITEM_DATA_DESCRIPTION,
	};

	struct item_ui_node : public hashtable::virtual_func_table, public hashtable::node
	{
		uint32_t unk0;
		uint32_t unk1;
		uint32_t unk2;
		uint32_t unk3;
		char**   name;	   // 540
		uint32_t unk4[135];
		char**   art;	   // 760 
		uint32_t unk5[7];
		char**   ubertip;  // 780
	};
	typedef hashtable::table<item_ui_node> item_ui_table_t;

	static uintptr_t search_item_table()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = s.search_string("GetItemName");
		ptr = *(uintptr_t*)(ptr + 0x05);
		if (s.get_version() > version_121b)
		{
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr += 5;
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr = convert_function(ptr);
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr = convert_function(ptr);
			ptr = next_opcode(ptr, 0xB9, 5);
			return *(uintptr_t*)(ptr + 1);
		}
		else
		{
			ptr = next_opcode(ptr, 0xC3, 1);
			ptr += 1;
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr = convert_function(ptr);
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr = convert_function(ptr);
			ptr = next_opcode(ptr, 0x8B, 6);
			return *(uintptr_t*)(ptr + 2) - 0x24;
		}
	}

	static uintptr_t get_item_table()
	{
		static uintptr_t table = search_item_table();
		return table;
	}

	static uint32_t hashid(uint32_t id)
	{
		static int box[] = {
			0x098AA3D0C, 0x0F67BCA9E, 0x0C46CA84C, 0x02AC9D845,
			0x09A1CF1DD, 0x06450148E, 0x08516213D, 0x0C0882BBF,
			0x0F10C2A9C, 0x09D7CF013, 0x0CD845F5E, 0x01D4BD837,
			0x01055F69A, 0x0A6A87DCD, 0x0312D8D9E, 0x0645A1CEC,
		};
		uint32_t hashlo = 0x7FED7FED;
		uint32_t hashhi = 0xEEEEEEEE;
		for (; id > 0;)
		{
			hashlo = (box[(id >> 4) & 0x0F] - box[id & 0x0F]) ^ (hashhi + hashlo);
			hashhi = hashhi + (id & 0xFF) + 32 * hashhi + hashlo + 3;
			id >>= 8;
		}
		return hashlo;
	}

	uint32_t  __cdecl EXGetItemDataString(uint32_t code, uint32_t type)
	{
		item_ui_table_t* table = (item_ui_table_t*)get_item_table();
		if (!table) {
			return jass::create_string("");
		}
		item_ui_node* ptr = table->get(hashid(code));
		if (!ptr) {
			return jass::create_string("");
		}
		char** buf = 0;
		switch (type) {
		case ITEM_DATA_ART:
			buf = ptr->art;
			break;
		case ITEM_DATA_UBERTIP:
			buf = ptr->ubertip;
			break;
		case ITEM_DATA_NAME:
			buf = ptr->name;
			break;
		}
		if (!buf || !*buf)
		{
			return jass::create_string("");
		}
		return jass::create_string(*buf);
	}

	bool  __cdecl EXSetItemDataString(uint32_t code, uint32_t type, uint32_t value)
	{
		item_ui_table_t* table = (item_ui_table_t*)get_item_table();
		if (!table) {
			return false;
		}
		item_ui_node* ptr = table->get(hashid(code));
		if (!ptr) {
			return false;
		}
		char** buf = 0;
		switch (type) {
		case ITEM_DATA_ART:
			buf = ptr->art;
			break;
		case ITEM_DATA_UBERTIP:
			buf = ptr->ubertip;
			break;
		case ITEM_DATA_NAME:
			buf = ptr->name;
			break;
		}

		if (!buf || !*buf)
		{
			return false;
		}
		string_pool.free((uintptr_t)*buf);
		const char* value_str = jass::from_string(value);
		if (value_str)
		{
			size_t      value_len = strlen(value_str);
			uintptr_t   value_buf = string_pool.malloc(value_len + 1);
			*buf = (char*)value_buf;
			if (value_buf)
			{
				strncpy_s(*buf, value_len + 1, value_str, value_len);
			}
		}
		else
		{
			*buf = 0;
		}
		return true;
	}

	void InitializeItemState()
	{
		jass::japi_add((uintptr_t)EXGetItemDataString, "EXGetItemDataString", "(II)S");
		jass::japi_add((uintptr_t)EXSetItemDataString, "EXSetItemDataString", "(IIS)B");
	}
}}}
