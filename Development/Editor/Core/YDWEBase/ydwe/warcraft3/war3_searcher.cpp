#include <ydwe/warcraft3/war3_searcher.h>
#include <ydwe/warcraft3/version.h>
#include <ydwe/util/singleton.h>

_BASE_BEGIN namespace warcraft3 {

	war3_searcher::war3_searcher()
		: _Mybase(::GetModuleHandleW(L"Game.dll"))
		, version_(search_version())
		, get_instance_(search_get_instance())
		, get_gameui_(search_get_gameui())
		, unit_handle_to_object_(search_unit_handle_to_object())
	{ }

	war3_searcher::war3_searcher(HMODULE hGameDll)
		: _Mybase(hGameDll)
		, version_(search_version())
		, get_instance_(search_get_instance())
		, get_gameui_(search_get_gameui())
		, unit_handle_to_object_(search_unit_handle_to_object())
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

	uintptr_t war3_searcher::unit_handle_to_object(uint32_t handle)
	{
		return ((uintptr_t(_fastcall*)(uint32_t))unit_handle_to_object_)(handle);
	}

	bool war3_searcher::is_gaming()
	{
		return (0 != get_gameui(0,0));
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

		return 1000*(((uint8_t*)ptr)[size]-'0') 
			+ 100*(((uint8_t*)ptr)[size+1]-'0') 
			+  10*(((uint8_t*)ptr)[size+2]-'0') 
			+     (((uint8_t*)ptr)[size+3]-'0');
#undef WARCRAFTIII_VERSION_STR
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
		// 搜索 GetGameUI
		//=========================================
		//=========================================
		// (1)
		//
		// push "(BB)V"
		// mov edx, "EnableSelect"
		// mov ecx, [EnableSelect函数的地址] <----
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

	namespace detail
	{
		uintptr_t unit_handle_table     = 0;
		uintptr_t get_unit_handle_table = 0;

		uintptr_t __fastcall unit_handle_to_object_120(uint32_t handle)
		{
			uintptr_t table = ((uintptr_t(_fastcall*)(uintptr_t))get_unit_handle_table)(*(uintptr_t*)unit_handle_table);
			if (table)
			{
				if (handle >= 0x100000)
				{
					return *(uintptr_t*)(*(uintptr_t*)(table + 0x19C) + 12 * handle - 0xBFFFFC);
				}
			}

			return 0;
		}
	}

	uintptr_t war3_searcher::search_unit_handle_to_object() const
	{
		uintptr_t unit_handle_to_object;

		//=========================================
		// (1)
		//
		// push offset "(Hunit;)R"
		// mov  edx, "GetUnitX"
		// mov  ecx, [GetUnitX函数的地址] <----
		// call BindNative
		//=========================================
		unit_handle_to_object = search_string("GetUnitX");
		unit_handle_to_object = *(uintptr_t*)(unit_handle_to_object + 0x05);

		if (version_ > version_121b)
		{
			//=========================================
			// (2)
			//  GetUnitX:
			//    mov     ecx, [esp+arg_0]
			//    sub     esp, 0Ch
			//    call    UnitHanldeToObject  <----
			//    test    eax, eax
			//    ...
			//    ...
			//=========================================
			unit_handle_to_object = next_opcode(unit_handle_to_object, 0xE8, 5);
			unit_handle_to_object = convert_function(unit_handle_to_object);

			return unit_handle_to_object;
		}
		else
		{
			//=========================================
			// (2)
			//  GetUnitX:
			//    push    ebp
			//    mov     ebp, esp
			//    sub     esp, 20h
			//    mov     ecx, dword_6F8722BC
			//    push    ebx
			//    xor     ebx, ebx
			//    push    edi
			//    mov     [ebp+var_4], ebx
			//    call    sub_6F29B2E0
			//=========================================
			unit_handle_to_object = next_opcode(unit_handle_to_object, 0x8B, 6);
			detail::unit_handle_table    = *(uintptr_t*)(unit_handle_to_object+2);

			unit_handle_to_object = next_opcode(unit_handle_to_object, 0xE8, 5);
			detail::get_unit_handle_table = convert_function(unit_handle_to_object);

			return (uintptr_t)detail::unit_handle_to_object_120;
		}
	}

	war3_searcher& get_war3_searcher()
	{
		return util::singleton_nonthreadsafe<war3_searcher>::instance();
	}
}}
