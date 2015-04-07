#include <base/win/process.h>
#include <base/hook/detail/inject_dll.h>
#include <base/util/dynarray.h>
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
			const boost::filesystem::path& dll_path)
		{
			if (boost::filesystem::exists(dll_path))
			{
#if !defined(DISABLE_DETOURS)
				return create_process_use_detour(application, command_line, inherit_handle, creation_flags, current_directory, startup_info, process_information, dll_path.string().c_str());
#else
				bool result = create_process_use_system(application, command_line, inherit_handle, creation_flags | CREATE_SUSPENDED, current_directory, startup_info, process_information);

				if (result) 
				{
					if (!hook::detail::inject_dll(process_information->hProcess, process_information->hThread, dll_path.c_str()))
					{
						result = false;
					}
					::ResumeThread(process_information->hThread);
				}

				return result;
#endif
			}
			else
			{
				return create_process_use_system(application, command_line, inherit_handle, creation_flags, current_directory, startup_info, process_information);
			}
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

	bool process::inject(const boost::filesystem::path& dll_path)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			inject_dll_ = dll_path;
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

	bool process::create(const boost::filesystem::path& application, const std::wstring& command_line, const boost::filesystem::path& current_directory)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			if (command_line.empty())
			{
				if (!detail::create_process(
						boost::filesystem::exists(application) ? application.c_str(): nullptr, 
						nullptr,
						inherit_handle_,
						NORMAL_PRIORITY_CLASS, 
						boost::filesystem::exists(current_directory) ? current_directory.c_str(): nullptr, 
						&si_, &pi_, inject_dll_
					))
				{
					return false;
				}
			}
			else
			{
				std::dynarray<wchar_t> command_line_buffer(command_line.size()+1);
				wcscpy_s(command_line_buffer.data(), command_line_buffer.size(), command_line.c_str());

				if (!detail::create_process(
						boost::filesystem::exists(application) ? application.c_str(): nullptr, 
						command_line_buffer.data(),
						inherit_handle_,
						NORMAL_PRIORITY_CLASS, 
						boost::filesystem::exists(current_directory) ? current_directory.c_str(): nullptr, 
						&si_, &pi_, inject_dll_
						))
				{
					return false;
				}
			}

			statue_ = PROCESS_STATUE_RUNNING;
			return true;
		}

		return false;
	}

	bool process::create(const boost::filesystem::path& application, const std::wstring& command_line)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			if (command_line.empty())
			{
				if (!detail::create_process(
					boost::filesystem::exists(application) ? application.c_str(): nullptr, 
					nullptr,
					inherit_handle_,
					NORMAL_PRIORITY_CLASS, 
					nullptr, 
					&si_, &pi_, inject_dll_
					))
				{
					return false;
				}
			}
			else
			{
				std::dynarray<wchar_t> command_line_buffer(command_line.size()+1);
				wcscpy_s(command_line_buffer.data(), command_line_buffer.size(), command_line.c_str());

				if (!detail::create_process(
					boost::filesystem::exists(application) ? application.c_str(): nullptr, 
					command_line_buffer.data(),
					inherit_handle_,
					NORMAL_PRIORITY_CLASS, 
					nullptr, 
					&si_, &pi_, inject_dll_
					))
				{
					return false;
				}
			}

			statue_ = PROCESS_STATUE_RUNNING;
			return true;
		}

		return false;
	}

	bool process::create(const std::wstring& command_line, const boost::filesystem::path& current_directory)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			if (command_line.empty())
			{
				if (!detail::create_process(
					nullptr, 
					nullptr,
					inherit_handle_,
					NORMAL_PRIORITY_CLASS, 
					boost::filesystem::exists(current_directory) ? current_directory.c_str(): nullptr, 
					&si_, &pi_, inject_dll_
					))
				{
					return false;
				}
			}
			else
			{
				std::dynarray<wchar_t> command_line_buffer(command_line.size()+1);
				wcscpy_s(command_line_buffer.data(), command_line_buffer.size(), command_line.c_str());

				if (!detail::create_process(
					nullptr, 
					command_line_buffer.data(),
					inherit_handle_,
					NORMAL_PRIORITY_CLASS, 
					boost::filesystem::exists(current_directory) ? current_directory.c_str(): nullptr, 
					&si_, &pi_, inject_dll_
					))
				{
					return false;
				}
			}

			statue_ = PROCESS_STATUE_RUNNING;
			return true;
		}

		return false;
	}

	bool process::create(const std::wstring& command_line)	
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			if (command_line.empty())
			{
				if (!detail::create_process(
					nullptr, 
					nullptr, 
					inherit_handle_,
					NORMAL_PRIORITY_CLASS, 
					nullptr, 
					&si_, &pi_, inject_dll_
					))
				{
					return false;
				}
			}
			else
			{
				std::dynarray<wchar_t> command_line_buffer(command_line.size()+1);
				wcscpy_s(command_line_buffer.data(), command_line_buffer.size(), command_line.c_str());

				if (!detail::create_process(
					nullptr, 
					command_line_buffer.data(),
					inherit_handle_,
					NORMAL_PRIORITY_CLASS, 
					nullptr, 
					&si_, &pi_, inject_dll_
					))
				{
					return false;
				}
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

	bool create_process(const boost::filesystem::path& application, const std::wstring& command_line, const boost::filesystem::path& current_directory, const boost::filesystem::path& inject_dll, PROCESS_INFORMATION* pi_ptr)
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
