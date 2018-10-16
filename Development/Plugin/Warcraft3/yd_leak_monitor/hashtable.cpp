#include "hashtable.h"

#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/hook.h>

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


	static int searchGlobalData()
	{
		uintptr_t ptr = base::warcraft3::get_war3_searcher().search_string("SaveInteger");
		ptr = *(uintptr_t*)(ptr + 0x05);
		ptr = base::warcraft3::next_opcode(ptr, 0x8B, 6);
		ptr = *(uintptr_t*)(ptr + 0x02);
		return ptr;
	}

	void tableEach(table* table, std::function<void(uint32_t*)> cb)
	{
		uint32_t* node = table->head;
		if ((int32_t)node <= 0) {
			return;
		}
		for (;;) {
			cb(node);
			node = (uint32_t*)node[table->step / 4 + 1];
			if ((int32_t)node <= 0) {
				return;
			}
		}
	}

	void hashtableEachHandle(std::function<void(uint32_t, uint32_t, uint32_t, uint32_t)> cb) {
		static uint32_t gd = searchGlobalData();
		uint32_t g = ((uint32_t*)*(uint32_t*)gd)[257];
		if (!g) {
			return;
		}
		tableEach((table *)(g + 0x04), [&](uint32_t* t) {
			tableEach((table *)((uint32_t)t + 0x1C), [&](uint32_t* k) {
				tableEach((table *)((uint32_t)k + 0xCC), [&](uint32_t* h) {
					cb(t[6], k[6], h[5], h[6]);
				});
			});
		});
	}
}
