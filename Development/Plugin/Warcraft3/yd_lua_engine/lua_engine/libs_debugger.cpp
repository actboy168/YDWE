#include <Windows.h>
#include <lua.hpp>
#include <algorithm>
#include <functional>
#include <base/filesystem.h>

namespace warcraft3::lua_engine::debugger {

#define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)

	typedef enum _KWAIT_REASON
	{
		Executive,
		FreePage,
		PageIn,
		PoolAllocation,
		DelayExecution,
		Suspended,
		UserRequest,
		WrExecutive,
		WrFreePage,
		WrPageIn,
		WrPoolAllocation,
		WrDelayExecution,
		WrSuspended,
		WrUserRequest,
		WrEventPair,
		WrQueue,
		WrLpcReceive,
		WrLpcReply,
		WrVertualMemory,
		WrPageOut,
		WrRendezvous,
		Spare2,
		Spare3,
		Spare4,
		Spare5,
		Spare6,
		WrKernel
	}KWAIT_REASON;

	typedef enum _SYSTEM_INFORMATION_CLASS {
		SystemBasicInformation = 0,
		SystemPerformanceInformation = 2,
		SystemTimeOfDayInformation = 3,
		SystemProcessInformation = 5,
		SystemProcessorPerformanceInformation = 8,
		SystemInterruptInformation = 23,
		SystemExceptionInformation = 33,
		SystemRegistryQuotaInformation = 37,
		SystemLookasideInformation = 45,
		SystemPolicyInformation = 134,
	} SYSTEM_INFORMATION_CLASS;

	typedef LONG KPRIORITY;

	typedef struct _CLIENT_ID
	{
		HANDLE UniqueProcess;
		HANDLE UniqueThread;
	}CLIENT_ID;
	typedef CLIENT_ID *PCLIENT_ID;

	typedef struct _LSA_UNICODE_STRING
	{
		USHORT  Length;
		USHORT  MaximumLength;
		PWSTR   Buffer;
	}LSA_UNICODE_STRING, *PLSA_UNICODE_STRING;
	typedef LSA_UNICODE_STRING UNICODE_STRING, *PUNICODE_STRING;

	typedef struct _VM_COUNTERS
	{
		ULONG PeakVirtualSize;                  //虚拟存储峰值大小；
		ULONG VirtualSize;                      //虚拟存储大小；
		ULONG PageFaultCount;                   //页故障数目；
		ULONG PeakWorkingSetSize;               //工作集峰值大小；
		ULONG WorkingSetSize;                   //工作集大小；
		ULONG QuotaPeakPagedPoolUsage;          //分页池使用配额峰值；
		ULONG QuotaPagedPoolUsage;              //分页池使用配额；
		ULONG QuotaPeakNonPagedPoolUsage;       //非分页池使用配额峰值；
		ULONG QuotaNonPagedPoolUsage;           //非分页池使用配额；
		ULONG PagefileUsage;                    //页文件使用情况；
		ULONG PeakPagefileUsage;                //页文件使用峰值；
	}VM_COUNTERS, *PVM_COUNTERS;

	typedef struct _SYSTEM_THREAD_INFORMATION {
		LARGE_INTEGER           KernelTime;
		LARGE_INTEGER           UserTime;
		LARGE_INTEGER           CreateTime;
		ULONG                   WaitTime;
		PVOID                   StartAddress;
		CLIENT_ID               ClientId;
		KPRIORITY               Priority;
		LONG                    BasePriority;
		ULONG                   ContextSwitchCount;
		ULONG                   State;
		KWAIT_REASON            WaitReason;
	}SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

	typedef struct _SYSTEM_PROCESS_INFORMATION {
		ULONG                     NextEntryOffset;
		ULONG                     NumberOfThreads;
		LARGE_INTEGER             Reserved[3];
		LARGE_INTEGER             CreateTime;
		LARGE_INTEGER             UserTime;
		LARGE_INTEGER             KernelTime;
		UNICODE_STRING            ImageName;
		KPRIORITY                 BasePriority;
		HANDLE                    ProcessId;
		HANDLE                    InheritedFromProcessId;
		ULONG                     HandleCount;
		ULONG                     Reserved2[2];
		ULONG                     PrivatePageCount;
		VM_COUNTERS               VirtualMemoryCounters;
		IO_COUNTERS               IoCounters;
		//SYSTEM_THREAD_INFORMATION Threads[0];
	} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

	typedef NTSTATUS(CALLBACK *PFN_NTQUERYSYSTEMINFORMATION)(SYSTEM_INFORMATION_CLASS SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

	bool QueryProcess(std::function<bool(const SYSTEM_PROCESS_INFORMATION*)> NtCBProcessInformation) {
		static HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
		if (hNtDll == NULL) {
			return false;
		}
		static PFN_NTQUERYSYSTEMINFORMATION pfnNtQSI = (PFN_NTQUERYSYSTEMINFORMATION)GetProcAddress(hNtDll, "NtQuerySystemInformation");
		if (pfnNtQSI == NULL) {
			return false;
		}
		std::unique_ptr<uint8_t[]> buffer;
		size_t size = 0x20000;
		size_t returnedSize = 0;
		NTSTATUS status = -1;
		do {
			buffer.reset(new uint8_t[(size + 7) / 8 * 8]);
			status = pfnNtQSI(SystemProcessInformation, buffer.get(), size, (PULONG)&returnedSize);
			if (status == STATUS_INFO_LENGTH_MISMATCH) {
				size = returnedSize;
			}
		} while (status == STATUS_INFO_LENGTH_MISMATCH);

		if (status != 0) {
			return false;
		}
		for (size_t offset = 0; offset < returnedSize;) {
			const SYSTEM_PROCESS_INFORMATION* ptr = (const SYSTEM_PROCESS_INFORMATION*)(buffer.get() + offset);
			if (ptr->ImageName.Buffer != 0) {
				if (NtCBProcessInformation(ptr)) {
					break;
				}
			}
			if (ptr->NextEntryOffset == 0)
			{
				break;
			}
			offset += ptr->NextEntryOffset;
		}
		return true;
	}

	std::wstring QueryProcessPath(const std::wstring& name)
	{
		DWORD pid = 0;
		QueryProcess([&](const SYSTEM_PROCESS_INFORMATION* info)->bool {
			std::wstring pname(info->ImageName.Buffer, info->ImageName.Length / sizeof(wchar_t));
			std::transform(pname.begin(), pname.end(), pname.begin(), ::towlower);
			if (pname == name) {
				pid = (DWORD)info->ProcessId;
				return true;
			}
			return false;
		});
		if (!pid) {
			return L"";
		}
		HANDLE proc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
		if (!proc) {
			return L"";
		}
		wchar_t buf[MAX_PATH];
		DWORD   len = MAX_PATH;
		if (!QueryFullProcessImageNameW(proc, 0, buf, &len)) {
			return L"";
		}
		return std::wstring(buf, len);
	}

#define DoString(L, s, n) \
	(luaL_loadbuffer(L, s, sizeof(s) - 1, "module '" #s "'") || (lua_insert(L, -(n)-1), lua_pcall(L, n, LUA_MULTRET, 0)))

	int open(lua_State* L, int port)
	{
		HMODULE m = GetModuleHandleW(L"debugger.dll");
		if (!m) {
			fs::path path = fs::path(QueryProcessPath(L"vscode-lua-debug.exe")).parent_path() / L"debugger.dll";
			if (fs::exists(path)) {
				m = LoadLibraryW(path.c_str());
				if (!m) {
					return 0;
				}
			}
		}
		lua_CFunction openf = (lua_CFunction)GetProcAddress(m, "luaopen_debugger");
		if (!openf) {
			return 0;
		}
		openf(L);
		lua_pushinteger(L, port);
		if (DoString(L, R"(
			local dbg, port = ... 
			dbg:listen('127.0.0.1', port)
			dbg:config [[
				{ "sourceCoding": "utf8" }
			]]
			return dbg
		)", 2)) {
			return 0;
		}
		return 1;
	}
}
