#pragma once

#include <base/config.h>
#include <stdint.h>

namespace base { namespace warcraft3 { namespace player {

	struct selection_node_t
	{
		uint32_t  unknown;
		struct selection_node_t* next;
		uint32_t  unit;
	};

	struct selection_list_t
	{
		void* virtual_table;
		uint32_t some_offset;
		void* data_pointer_a;
		struct selection_node_t* phead;
		uint32_t unit_count;
	};

	struct selection_t
	{
		void*     virtual_table;
		uint32_t  unk004;
		uint32_t  unk008;
		uint32_t  unk00c;
		uint32_t  unk010;
		struct selection_list_t list_array[20];
		uint32_t  unk1a4;
		uint32_t* item_last_selected;
		uint32_t  player_id;
		uint32_t  unk1b0;
		uint32_t  unk1b4;
		uint32_t  unk1b8;
		uint32_t  unk1bc;
		uint32_t  unk1c0;
		struct selection_list_t list_a;
		uint32_t* unit_b1;
		uint32_t  unit_id;
		uint32_t  unit;
		uint32_t* item_selected;
		struct selection_list_t list_b;
		uint32_t  unkc1;
		uint32_t  unkc2;
		uint32_t  unkc3;
		uint32_t  unkc4;
		uint32_t  unkc5;
		uint32_t  unkc6;
		uint32_t  unkc7;
	};

	_BASE_API uintptr_t from_id(uint32_t pid);
	_BASE_API uint32_t  local_pid();
	_BASE_API uintptr_t local();
	_BASE_API selection_t* selection(uintptr_t player);
}}}
