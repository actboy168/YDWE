#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/version.h>
#include <base/warcraft3/hashtable.h>
#include <base/util/singleton.h>
#include <base/hook/fp_call.h>

namespace base { namespace warcraft3 {

	war3_searcher::war3_searcher()
		: _Mybase(::GetModuleHandleW(L"Game.dll"))
		, version_(search_version())
		, get_instance_(search_get_instance())
		, get_gameui_(search_get_gameui())
	{ }
	
	war3_searcher::war3_searcher(HMODULE hGameDll)
		: _Mybase(hGameDll)
		, version_(search_version())
		, get_instance_(search_get_instance())
		, get_gameui_(search_get_gameui())
	{ }
	
	uint32_t war3_searcher::get_version() const
	{
		return version_;
	}
	
	uint32_t war3_searcher::get_instance(uint32_t index)
	{
		return ((uint32_t(_fastcall*)(uint32_t))get_instance_)(index);
	}
	
	uint32_t war3_searcher::get_gameui(uint32_t unk0, uint32_t unk1)
	{
		return ((uint32_t(_fastcall*)(uint32_t, uint32_t))get_gameui_)(unk0, unk1);
	}
	
	bool war3_searcher::is_gaming()
	{
		return (0 != get_gameui(0, 0));
	}
	
	uintptr_t war3_searcher::current_function(uintptr_t ptr)
	{
		static uint32_t nop = get_version() > version_121b ? 0xCCCCCCCC : 0x90909090;
		for (; nop != *(uint32_t*)ptr; --ptr)
			;
		ptr += 4;
		return ptr;
	}
	
	uint32_t war3_searcher::search_version() const
	{
		static const char warcraft3_version_string[] = "Warcraft III (build ";
	
		uintptr_t ptr;
		size_t size = sizeof(warcraft3_version_string) - 1;
		ptr = search_string_ptr(warcraft3_version_string, size);
		if (!ptr)
		{
			return 0;
		}
		uint32_t n = 0;
		ptr += size;
		while (isdigit(*(uint8_t*)ptr))
		{
			n = n * 10 + *(uint8_t*)ptr - '0';
			ptr++;
		}
	
		return n;
	}

	uintptr_t war3_searcher::search_get_instance() const
	{
		uintptr_t get_instance;
		//=========================================
		//  (1)
		//
		//    push    493E0h
		//    push    1
		//    push    1
		//    push    0
		//    mov     edx, offset s_Config ; "config"
		//    mov     ecx, esi
		//    call    UnknowFunc  <----
		//=========================================
		get_instance = search_string("config");
		get_instance += sizeof uintptr_t;

		get_instance = next_opcode(get_instance, 0xE8, 5);
		get_instance = convert_function(get_instance);
		//=========================================
		//  (2)
		//
		//  UnknowFunc:
		//    push    esi
		//    mov     esi, edx
		//    call    jGetVMInstance <---
		//=========================================
		get_instance = next_opcode(get_instance, 0xE8, 5);
		get_instance = convert_function(get_instance);
		//=========================================
		//  (3)
		//
		//  jGetVMInstance:
		//    jmp    jGetVMInstance2 <----
		//=========================================
		get_instance = convert_function(get_instance);
		//=========================================
		//  (4)
		//
		//  jGetVMInstance2:
		//    push    esi
		//    mov     esi, ecx
		//    mov     ecx, 5
		//    call    GetInstance  <----
		//    push    esi
		//    mov     ecx, eax
		//    call    UnknowFunc
		//    pop     esi
		//    retn
		//=========================================
		get_instance = next_opcode(get_instance, 0xE8, 5);
		get_instance = convert_function(get_instance);
		return get_instance;
	}

	uintptr_t war3_searcher::search_get_gameui() const
	{
		uintptr_t get_gameui;

		//=========================================
		// ËÑË÷ GetGameUI
		//=========================================
		//=========================================
		// (1)
		//
		// push "(BB)V"
		// mov edx, "EnableSelect"
		// mov ecx, [EnableSelectº¯ÊýµÄµØÖ·] <----
		// call BindNative
		//=========================================
		get_gameui = search_string("EnableSelect");
		get_gameui = *(uintptr_t*)(get_gameui + 0x05);

		//=========================================
		// (2)
		//  EnableSelect:
		//    xor     edx, edx
		//    lea     ecx, [edx+1]
		//    call    GetGameUI  <----
		//    test    eax, eax
		//    jz      short to_retn
		//    mov     ecx, [esp+arg_4]
		//    mov     edx, [esp+arg_0]
		//    push    ecx
		//    push    edx
		//    call    sub_6F2F8710
		//  to_retn:
		//    retn
		//=========================================
		get_gameui = next_opcode(get_gameui, 0xE8, 5);
		get_gameui = convert_function(get_gameui);

		return get_gameui;
	}

	war3_searcher& get_war3_searcher()
	{
		return singleton_nonthreadsafe<war3_searcher>::instance();
	}

	struct mapping_objectid
	{
		uint32_t unk00;
		uint32_t unk01;
		uint32_t unk02;
		uint32_t unk03;
		uint32_t unk04;
		uint32_t unk05;
		uint32_t unk06;
		uint32_t unk07;
		uint32_t unk08;
		uint32_t unk09;
		uint32_t unk0A;
		uint32_t unk0B;
		uint32_t unk0C;
		uint32_t unk0D;
		uint32_t unk0E;
		uint32_t unk0F;
	};

	namespace detail
	{
		mapping_objectid** search_mapping_objectid()
		{
			war3_searcher& s = get_war3_searcher();
			uintptr_t ptr = 0;
			if (s.get_version() >= version_127a)
			{
				ptr = s.search_string("D:\\BuildServer\\3\\work-git\\warcraft3-repository\\Engine\\Source\\Tempest/tempest_thread.h");
			}
			else if (s.get_version() > version_121b)
			{
				ptr = s.search_string("e:\\Drive1\\temp\\buildwar3x\\Engine\\Source\\Tempest/tempest_thread.h");
			}
			else
			{
				ptr = s.search_string("..\\Tempest/tempest_thread.h");
			}

			ptr += 0x04;
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr += 0x05;
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr = convert_function(ptr);
			ptr = next_opcode(ptr, 0x89, 6);
			ptr = *(uintptr_t*)(ptr + 0x02);
			return (mapping_objectid**)(ptr);
		}
	}

	uintptr_t find_objectid_64(const objectid_64& id)
	{
		static mapping_objectid** table_pptr = detail::search_mapping_objectid();

		mapping_objectid* table_ptr = *table_pptr;
		if (!table_ptr)
			return 0;

		if (id.a >> 31)
		{
			if ((id.a & 0x7FFFFFFF) < table_ptr->unk0F)
			{
				if (table_ptr->unk0B && *(uintptr_t*)(table_ptr->unk0B + 8 * id.a) == -2)
				{
					uintptr_t v4 = *(uintptr_t*)(table_ptr->unk0B + 8 * id.a + 4);
					if (v4 && (*(uintptr_t*)(v4 + 0x18) == id.b))
					{
						return v4;
					}
				}
			}
		}
		else
		{
			if (id.a < table_ptr->unk07)
			{
				if (table_ptr->unk03 && *(uintptr_t*)(table_ptr->unk03 + 8 * id.a) == -2)
				{
					uintptr_t v5 = *(uintptr_t*)(table_ptr->unk03 + 8 * id.a + 4);
					if (v5 && (*(uintptr_t*)(v5 + 0x18) == id.b))
					{
						return v5;
					}
				}
			}
		}
		return 0;
	}

	namespace detail
	{
		bool is_valid_ptr(uintptr_t ptr)
		{
			return (ptr >= get_war3_searcher().base() && ptr < (get_war3_searcher().base() + get_war3_searcher().size()));
		}
	}

	const char* get_class_name(uintptr_t ptr)
	{
		if (ptr && !::IsBadReadPtr(reinterpret_cast<const void*>(ptr), sizeof(uintptr_t)))
		{
			ptr = *(uintptr_t*)ptr - 0x04;
			if (detail::is_valid_ptr(ptr))
			{
				ptr = *(uintptr_t*)ptr + 0x0C;
				if (detail::is_valid_ptr(ptr))
				{
					ptr = *(uintptr_t*)ptr + 0x08;
					return (const char*)ptr;
				}
			}
		}
		return nullptr;
	}

	uintptr_t get_vfn_ptr(const char* name)
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = s.search_string_ptr(name, strlen(name));
		if (!ptr)
			return 0;
		ptr -= 0x08;
		ptr = s.search_int_in_rdata(ptr);
		if (!ptr)
			return 0;
		ptr -= 0x0C;
		ptr = s.search_int_in_rdata(ptr);
		if (!ptr)
			return 0;
		ptr += 0x04;
		return ptr;
	}

	uint32_t get_object_type(uintptr_t ptr)
	{
		return this_call<uint32_t>(*(uintptr_t*)(*(uintptr_t*)ptr + 0x1C), ptr);
	}

	uintptr_t handle_to_object(uint32_t handle)
	{
		if (handle < 0x100000)
		{
			return 0;
		}

		uintptr_t object = (uintptr_t)(*get_jass_vm()->handle_table)->table.at(3 * (handle - 0x100000) + 1);

		if (!object)
		{
			return 0;
		}

		uintptr_t ptr = find_objectid_64(*(objectid_64*)(object + 0x0C));
		if (!ptr || (*(uintptr_t*)(ptr + 0x0C) != '+agl') || *(uintptr_t*)(ptr + 0x20))
		{
			return 0;
		}

		return object;
	}

	uint32_t object_to_handle(uintptr_t obj)
	{
		hashtable::reverse_table& table = (*get_jass_vm()->handle_table)->table;
		for (uint32_t i = 1; i < table.size*3; i += 3)
		{
			if (obj == (uintptr_t)table.at(i))
			{
				return 0x100000 + (i - 1) / 3;
			}
		}
		return 0;
	}
}}
