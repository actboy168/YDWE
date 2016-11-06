#include <base/win/process.h>
#include <base/hook/detail/inject_dll.h>   
#include <base/hook/replace_import.h>
#include <base/util/dynarray.h>	  
#include <base/util/foreach.h>
#include <Windows.h>
#include <memory>
#include <strsafe.h>
#include <cassert>
#if !defined(DISABLE_DETOURS)
#include <detours.h>	  
#pragma comment(lib, "detours.lib")
#endif

namespace base { namespace win {

	namespace detail 
	{
		bool create_process_use_system(
			const wchar_t*        application, 
			wchar_t*              command_line,
			bool                  inherit_handle,
			uint32_t              creation_flags,
			const wchar_t*        current_directory,
			LPSTARTUPINFOW        startup_info,
			LPPROCESS_INFORMATION process_information)
		{
			return !!::CreateProcessW(
				application, command_line, 
				NULL, NULL, inherit_handle, creation_flags, NULL,
				current_directory, 
				startup_info, 
				process_information);
		}

#if !defined(DISABLE_DETOURS)
		bool create_process_use_detour(
			const wchar_t*        application, 
			wchar_t*              command_line,
			bool                  inherit_handle,
			uint32_t              creation_flags,
			const wchar_t*        current_directory,
			LPSTARTUPINFOW        startup_info,
			LPPROCESS_INFORMATION process_information,
			const char*           dll_path)
		{
			return !!DetourCreateProcessWithDllW(
				application, command_line, 
				NULL, NULL, inherit_handle, creation_flags, NULL,
				current_directory, 
				startup_info, 
				process_information, 
				dll_path, 
				NULL);
		}
#endif

		bool create_process(
			const wchar_t*                 application, 
			wchar_t*                       command_line,
			bool                           inherit_handle,
			uint32_t                       creation_flags,
			const wchar_t*                 current_directory,
			LPSTARTUPINFOW                 startup_info,
			LPPROCESS_INFORMATION          process_information,
			const fs::path& inject_dll,
			const std::map<std::string, fs::path>& replace_dll)
		{
			bool need_pause = !replace_dll.empty();
#if defined(DISABLE_DETOURS)  	
			need_pause = need_pause || fs::exists(inject_dll);
#endif
			bool suc = false;
			if (fs::exists(inject_dll))
			{
#if !defined(DISABLE_DETOURS)
				suc = create_process_use_detour(application, command_line, inherit_handle, need_pause ? (creation_flags | CREATE_SUSPENDED) : creation_flags, current_directory, startup_info, process_information, inject_dll.string().c_str());
#else
				assert(need_pause);
				suc = create_process_use_system(application, command_line, inherit_handle, creation_flags | CREATE_SUSPENDED, current_directory, startup_info, process_information);
				if (suc) 
				{
					hook::detail::inject_dll(process_information->hProcess, process_information->hThread, inject_dll.c_str());
				}
#endif
			}
			else
			{
				suc = create_process_use_system(application, command_line, inherit_handle, need_pause ? (creation_flags | CREATE_SUSPENDED) : creation_flags, current_directory, startup_info, process_information);
			}

			if (suc && !replace_dll.empty())
			{
				foreach(auto it, replace_dll)
				{
					hook::ReplaceImport(process_information->hProcess, it.first.c_str(), it.second.string().c_str());
				}
			}

			if (suc && need_pause)
			{
				if (!(creation_flags & CREATE_SUSPENDED))
				{
					::ResumeThread(process_information->hThread);
				}
			}
			return suc;
		}
	}

	namespace process_helper
	{
		bool wait(HANDLE process, uint32_t timeout)
		{
			return (::WaitForSingleObject(process, timeout) == WAIT_OBJECT_0);
		}

		uint32_t exit_code(HANDLE process)
		{
			uint32_t tmp_exit_code = 0;

			if (!::GetExitCodeProcess(process, reinterpret_cast<LPDWORD>(&tmp_exit_code))) 
			{
				return 0;
			}

			return tmp_exit_code;
		}

		bool is_running(HANDLE process) 
		{
			if (exit_code(process) == STILL_ACTIVE) 
			{
				return wait(process, 0);
			}

			return false;
		}

		bool kill(HANDLE process, int exit_code, uint32_t timeout)
		{
			bool result = (::TerminateProcess(process, exit_code) != FALSE);
			if (result && timeout)
			{
				return wait(process, timeout);
			}

			return result;
		}
	}

	process::process()
		: statue_(PROCESS_STATUE_READY)
		, inherit_handle_(false)
	{
		si_.cb = sizeof STARTUPINFOW;
		::GetStartupInfoW(&si_);
		si_.dwFlags = 0;
		memset(&pi_, 0 ,sizeof PROCESS_INFORMATION);
	}

	process::~process()
	{
		close();
	}

	bool process::inject(const fs::path& dllpath)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			inject_dll_ = dllpath;
			return true;
		}

		return false;
	}

	bool process::replace(const fs::path& dllpath, const char* dllname)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			replace_dll_[dllname] = dllpath;
			return true;
		}
		return false;
	}

	bool process::hide_window()
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			si_.dwFlags |= STARTF_USESHOWWINDOW;
			si_.wShowWindow = SW_HIDE;
			return true;
		}

		return false;
	}

	bool process::redirect(HANDLE std_input, HANDLE std_output, HANDLE std_error)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			if (std_input || std_output || std_error)
			{
				si_.dwFlags |= STARTF_USESTDHANDLES;
				inherit_handle_ = true;

				if (std_input)
				{
					::SetHandleInformation(std_input, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
					si_.hStdInput = std_input;
				}

				if (std_output)
				{
					::SetHandleInformation(std_output, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
					si_.hStdOutput = std_output;
				}

				if (std_error)
				{
					::SetHandleInformation(std_error, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
					si_.hStdError = std_error;
				}

				return true;
			}
		}

		return false;
	}

	bool process::create(const std::optional<fs::path>& application, const std::wstring& command_line, const std::optional<fs::path>& current_directory)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			std::dynarray<wchar_t> command_line_buffer(command_line.size() + 1);
			wcscpy_s(command_line_buffer.data(), command_line_buffer.size(), command_line.c_str());

			if (!detail::create_process(
				application? application.get().c_str(): nullptr,
				command_line_buffer.data(),
				inherit_handle_,
				NORMAL_PRIORITY_CLASS,
				current_directory ? current_directory.get().c_str() : nullptr,
				&si_, &pi_, inject_dll_, replace_dll_
				))
			{
				return false;
			}
			statue_ = PROCESS_STATUE_RUNNING;
			return true;
		}
		return false;
	}

	uint32_t process::wait()
	{
		if (statue_ == PROCESS_STATUE_RUNNING)
		{
			process_helper::wait(pi_.hProcess, INFINITE);
			return process_helper::exit_code(pi_.hProcess);
		}

		return 0;
	}

	bool process::wait(uint32_t timeout)
	{
		if (statue_ == PROCESS_STATUE_RUNNING)
		{
			return process_helper::wait(pi_.hProcess, timeout);
		}

		return false;
	}
	
	bool process::is_running()
	{
		if (statue_ == PROCESS_STATUE_RUNNING)
		{
			return process_helper::is_running(pi_.hProcess);
		}

		return false;
	}

	bool process::kill(uint32_t timeout)
	{
		if (statue_ == PROCESS_STATUE_RUNNING)
		{
			return process_helper::kill(pi_.hProcess, 0, timeout);
		}

		return false;
	}

	bool process::close()
	{
		if (statue_ == PROCESS_STATUE_RUNNING)
		{
			statue_ = PROCESS_STATUE_READY;
			::CloseHandle(pi_.hThread);
			::CloseHandle(pi_.hProcess);
			return true;
		}

		return false;
	}

	bool process::release(PROCESS_INFORMATION* pi_ptr)
	{
		if (statue_ == PROCESS_STATUE_RUNNING)
		{
			statue_ = PROCESS_STATUE_READY;
			*pi_ptr = pi_;
			return true;
		}

		return false;
	}

	int process::id() const
	{
		if (statue_ == PROCESS_STATUE_RUNNING)
		{
			return (int)pi_.dwProcessId;
		}
		return -1;
	}

	bool create_process(const fs::path& application, const std::wstring& command_line, const fs::path& current_directory, const fs::path& inject_dll, PROCESS_INFORMATION* pi_ptr)
	{
		process p;

		p.inject(inject_dll);

		if (p.create(application, command_line, current_directory))
		{
			if (pi_ptr)
			{ 
				p.release(pi_ptr);
			}
			return true;
		}

		return false;
	}
}}
