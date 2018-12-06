#include "hashtable.h"

#include <warcraft3/war3_searcher.h>
#include <warcraft3/jass.h>
#include <warcraft3/jass/hook.h>
#include <warcraft3/hashtable.h>
#include <base/hook/inline.h>
#include <base/hook/fp_call.h>
#include <map>

namespace ht {
	struct bucket {
		uint32_t  step;
		uint32_t* tail;
		uint32_t* head;
	};

	struct table {
		uint32_t  unk0;
		uint32_t  step;
		uint32_t* tail;
		uint32_t* head;
		uint32_t  unk4;
		uint32_t  unk5;
		uint32_t  unk6;
		bucket*   buckets;
		uint32_t  unk8;
		uint32_t  mask;
	};


	static uintptr_t searchGlobalData()
	{
		uintptr_t ptr = base::warcraft3::get_war3_searcher().search_string("SaveInteger");
		ptr = *(uintptr_t*)(ptr + 0x05);
		ptr = base::warcraft3::next_opcode(ptr, 0x8B, 6);
		ptr = *(uintptr_t*)(ptr + 0x02);
		return ptr;
	}

	static uint32_t* getGlobalData()
	{
		static uint32_t gd = searchGlobalData();
		return (uint32_t*)*(uint32_t*)gd;
	}

	void hashtableEachHandle(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t)> cb) {
		uint32_t g = getGlobalData()[257];
		if (!g) {
			return;
		}
		typedef base::warcraft3::hashtable::table<base::warcraft3::hashtable::reverse_node> Table;

		for (auto& t : *(Table*)(g + 0x04)) {
			for (auto& k : *(Table*)((uint8_t*)&t + 0x1C)) {
				for (auto& h : *(Table*)((uint8_t*)&k + 0xCC)) {
					cb(t.value, k.value, h.key, h.value);
				}
			}
		}
	}

	uint32_t objectToHandle(base::warcraft3::handle_table_t* hts, uintptr_t object) {
		if (!object) {
			return 0;
		}
		uintptr_t ptr = base::warcraft3::find_objectid_64(*(base::warcraft3::objectid_64*)(object + 0x0C));
		if (!ptr || (*(uintptr_t*)(ptr + 0x0C) != '+agl') || *(uintptr_t*)(ptr + 0x20)) {
			return 0;
		}
		auto node = hts->table.find(*(uint32_t*)(object + 0x10));
		if (!node) {
			return 0;
		}
		if (object != (uintptr_t)hts->table.at(3 * (node->value - 0x100000) + 1)) {
			return 0;
		}
		return node->value;
	}

	uint32_t objectToHandle(uintptr_t object) {
		base::warcraft3::handle_table_t* hts = (base::warcraft3::handle_table_t*)getGlobalData()[7];
		if (!hts) {
			return 0;
		}
		return objectToHandle(hts, object);
	}

	static uintptr_t searchCreateHandle() {
		uintptr_t ptr = base::warcraft3::get_war3_searcher().search_string("Player");
		ptr = *(uintptr_t*)(ptr + 0x05);
		ptr = base::warcraft3::next_opcode(ptr, 0xE8, 5);
		ptr += 5;
		ptr = base::warcraft3::next_opcode(ptr, 0xE8, 5);
		ptr += 5;
		ptr = base::warcraft3::next_opcode(ptr, 0xE8, 5);
		return base::warcraft3::convert_function(ptr);
	}

	std::map<uint32_t, std::vector<base::warcraft3::jass::opcode*>> handlepos;

	uintptr_t realCreateHandle = 0;
	static uint32_t __fastcall fakeCreateHandle(base::warcraft3::handle_table_t* hts, uint32_t edx, uint32_t object, uint32_t unk) {
		uint32_t handle = objectToHandle(object);
		if (handle) {
			return handle;
		}
		handle = base::fast_call<uint32_t>(realCreateHandle, hts, edx, object, unk);
		handlepos[handle] = base::warcraft3::jass::stackwalker();
		return handle;
	}

	std::vector<base::warcraft3::jass::opcode*> getHandlePos(uint32_t handle) {
		auto it = handlepos.find(handle);
		if (it == handlepos.end()) {
			return std::vector<base::warcraft3::jass::opcode*>();
		}
		return it->second;
	}

	void initialize() {
		realCreateHandle = searchCreateHandle();
		base::hook::install(&realCreateHandle, (uintptr_t)fakeCreateHandle);
	}
}
