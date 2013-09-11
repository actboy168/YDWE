#include <base/hook/detail/inject_dll.h>
#include <base/hook/assembler/writer.h>
#include <windows.h>
#include <cstdint>
#include <strsafe.h>

namespace base { namespace hook { namespace detail {
	bool inject_dll(HANDLE process_handle, HANDLE thread_handle, const wchar_t* dll_name)
	{
		struct Code : public assembler::writer<128>
		{
			wchar_t  dll_path[512];
		} code;

		CONTEXT cxt = {0};
		cxt.ContextFlags = CONTEXT_FULL;
		if (!::GetThreadContext(thread_handle, &cxt))
		{
			return false;
		}

		uintptr_t code_base = (cxt.Esp - sizeof(code)) & ~0x1Fu;        // Cache-line align.

		if (dll_name) 
		{
			if (FAILED(::StringCchCopyW(code.dll_path, _countof(code.dll_path), dll_name)))
			{
				assert(false);
				return false;
			}

			uintptr_t pfLoadLibrary = (uintptr_t)::GetProcAddress(::GetModuleHandleW(L"Kernel32"), "LoadLibraryW");
			if (!pfLoadLibrary)
			{
				assert(false);
				return false;
			}

			code.push(code_base + offsetof(Code, dll_path));
			code.call(pfLoadLibrary, code_base + code.size());
		}

		code.mov(assembler::eax, cxt.Eax);
		code.mov(assembler::ebx, cxt.Ebx);
		code.mov(assembler::ecx, cxt.Ecx);
		code.mov(assembler::edx, cxt.Edx);
		code.mov(assembler::esi, cxt.Esi);
		code.mov(assembler::edi, cxt.Edi);
		code.mov(assembler::ebp, cxt.Ebp);
		code.mov(assembler::esp, cxt.Esp);
		code.jmp(cxt.Eip, code_base + code.size());

		cxt.Esp = code_base - 4;
		cxt.Eip = code_base;

		DWORD nProtect = 0;
		if (!::VirtualProtectEx(process_handle, (void*)code_base, sizeof Code, PAGE_EXECUTE_READWRITE, &nProtect)) 
		{
			return false;
		}

		DWORD nWritten = 0;
		if (!::WriteProcessMemory(process_handle, (void*)code_base, &code, sizeof Code, &nWritten)) 
		{
			return false;
		}

		if (!::FlushInstructionCache(process_handle, (void*)code_base, sizeof Code)) 
		{
			return false;
		}

		if (!::SetThreadContext(thread_handle, &cxt))
		{
			return false;
		} 

		return true;
	}
}}}
