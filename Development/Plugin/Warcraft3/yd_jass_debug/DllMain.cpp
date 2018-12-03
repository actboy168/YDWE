#include <windows.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/hook/inline.h>
#include <base/hook/fp_call.h>
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/opcode.h>
#include <base/warcraft3/jass/hook.h>
#include <base/util/console.h>
#include <bee/utility/format.h>
#include <base/path/ydwe.h>
#include <iostream>
#include <ctype.h>
#include "handle_scanner.h"
#include "hashtable.h"

namespace base { namespace warcraft3 { namespace jdebug {
	
	uintptr_t search_jass_vmmain()
	{
		war3_searcher& s = get_war3_searcher();
		uintptr_t ptr = 0;

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
		ptr = s.search_string("config");
		ptr += sizeof uintptr_t;
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr = convert_function(ptr);
		//=========================================
		//  (2)
		//
		//  UnknowFunc:
		//    push    esi
		//    mov     esi, edx
		//    call    GetVMInstance
		//    cmp     [esp+4+arg_8], 0
		//    mov     ecx, eax
		//    jz      short loc_6F44C170
		//    cmp     dword ptr [ecx+20h], 0
		//    jz      short loc_6F44C170
		//    call    UnknowFunc2         <----
		//=========================================
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr += 5;
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr = convert_function(ptr);
		//=========================================
		//  (3)
		//
		//  UnknowFunc2:
		//    mov     eax, [ecx+20h]
		//    push    0
		//    push    493E0h
		//    push    0
		//    push    eax
		//    call    JassVMMain    <----
		//    retn
		//=========================================
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr = convert_function(ptr);
		return ptr;
	}

	uintptr_t real_jass_vmmain = 0;

	struct jass::opcode* current_opcode(base::warcraft3::jass_vm_t* vm)
	{
		return vm->opcode - 1;
	}

	struct jass::opcode* show_pos(struct jass::opcode* current_op)
	{
		struct jass::opcode *op;
		for (op = current_op; op->op != jass::OPTYPE_FUNCTION; --op)
		{ }

		std::cout << "    [" << jass::from_stringid(op->arg) << ":" << current_op  - op << "]" << std::endl;
		return op;
	}

	void show_error(base::warcraft3::jass_vm_t* vm, const std::string& msg)
	{
		base::console::enable();
		std::cout << "---------------------------------------" << std::endl;
		std::cout << "               Jass错误                " << std::endl;
		std::cout << "---------------------------------------" << std::endl;
		std::cout << msg << std::endl;
		std::cout << std::endl;
		std::cout << "stack traceback:" << std::endl;

		for (auto& cur : jass::stackwalker(vm)) {
			show_pos(cur);
		}
		std::cout << "---------------------------------------" << std::endl;
	}

	uint32_t __fastcall fake_jass_vmmain(base::warcraft3::jass_vm_t* vm, uint32_t edx, uint32_t opcode, uint32_t unk2, uint32_t limit, uint32_t unk4)
	{
		uint32_t result = base::fast_call<uint32_t>(real_jass_vmmain, vm, edx, opcode, unk2, limit, unk4);

		switch (result)
		{
		case 1:
		case 3:
		case 4:
			break;
		case 2:
			show_error(vm, "超过了字节码限制");
			break;
		case 6:
		{
			jass::opcode* op = current_opcode(vm);
			if (op->op == jass::OPTYPE_PUSH)
			{
				show_error(vm, bee::format("栈 [0x02X] 没有初始化就使用", op->r3));
			}
			else
			{
				assert(op->op == jass::OPTYPE_MOVRV);
				show_error(vm, bee::format("变量 '%s' 没有初始化就使用", jass::from_stringid(op->arg)));
			}
			break;
		}
		case 7:
			show_error(vm, "使用零作为除数");
			break;
		default:
			show_error(vm, bee::format("未知错误 (%d).", result));
			break;
		}
		return result;
	}

	static fs::path getPath(const char* filename) {
		try {
			for (;*filename && isspace((unsigned char)*filename) ;++filename)
			{ }
			fs::path path = bee::u2w(filename);
			if (path.is_absolute()) {
				return path;
			}
			return base::path::ydwe(false) / "logs" / path;
		}
		catch (...) {
		}
		return fs::path();
	}
	
	void EXDebugOpcode(const char* filename) {
		jass::opcode* op = base::warcraft3::jass::currentpos();
		if (op) {
			for (; op->op > jass::OPTYPE_MINLIMIT && op->op < jass::OPTYPE_MAXLIMIT; --op)
			{ }
			jass::dump_opcode(op, getPath(filename).c_str());
		}
	}

	void EXDebugHandle(const char* filename) {
		std::fstream fs(getPath(filename), std::ios::out);
		if (fs) {
			handles::scanner(fs, true);
		}
	}

	void EXDebugLeak(const char* filename) {
		std::fstream fs(getPath(filename), std::ios::out);
		if (fs) {
			handles::scanner(fs, false);
		}
	}

	static uintptr_t RealGetLocalizedHotkey = 0;
	uint32_t __cdecl FakeGetLocalizedHotkey(uint32_t s)
	{
		const char* str = base::warcraft3::jass::from_string(s);
		if (str && strncmp(str, "ydwe::", 6) == 0) {
			if (strncmp(str + 6, "opcode:", 7) == 0) {
				EXDebugOpcode(str + 6 + 7);
			}
			else if (strncmp(str + 6, "handle:", 7) == 0) {
				EXDebugHandle(str + 6 + 7);
			}
			else if (strncmp(str + 6, "leak:", 5) == 0) {
				EXDebugLeak(str + 6 + 5);
			}
		}
		return base::c_call<uint32_t>(RealGetLocalizedHotkey, s);
	}

	bool initialize()
	{
		ht::initialize();
		base::warcraft3::jass::async_hook("GetLocalizedHotkey", &RealGetLocalizedHotkey, (uintptr_t)FakeGetLocalizedHotkey);
		real_jass_vmmain = search_jass_vmmain();
		return base::hook::install(&real_jass_vmmain, (uintptr_t)fake_jass_vmmain);
	}
}}}

void Initialize()
{
	base::warcraft3::jdebug::initialize();
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(module);
	}

	return TRUE;
}
