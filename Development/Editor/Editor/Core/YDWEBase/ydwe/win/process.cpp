#include <ydwe/win/process.h>
#include <ydwe/hook/detail/inject_dll.h>
#include <Windows.h>
#include <memory>
#include <strsafe.h>
#include <cassert>
#include <detours.h>
#pragma comment(lib, "detours.lib")

namespace ydwe { namespace win {

	namespace detail 
	{
		bool create_process_use_system(
			const wchar_t*        application, 
			wchar_t*              command_line,
			uint32_t              creation_flags,
			const wchar_t*        current_directory,
			LPSTARTUPINFOW        startup_info,
			LPPROCESS_INFORMATION process_information)
		{
			return !!::CreateProcessW(
				application, command_line, 
				NULL, NULL, FALSE, creation_flags, NULL, 
				current_directory, 
				startup_info, 
				process_information);
		}

		bool create_process_use_detour(
			const wchar_t*        application, 
			wchar_t*              command_line,
			uint32_t              creation_flags,
			const wchar_t*        current_directory,
			LPSTARTUPINFOW        startup_info,
			LPPROCESS_INFORMATION process_information,
			const char*           dll_path)
		{
			return !!DetourCreateProcessWithDllW(
				application, command_line, 
				NULL, NULL, FALSE, creation_flags, NULL, 
				current_directory, 
				startup_info, 
				process_information, 
				dll_path, 
				NULL);
		}


		bool create_process(
			const wchar_t*                 application, 
			wchar_t*                       command_line,
			uint32_t                       creation_flags,
			const wchar_t*                 current_directory,
			LPSTARTUPINFOW                 startup_info,
			LPPROCESS_INFORMATION          process_information,
			const boost::filesystem::path& dll_path)
		{
			if (boost::filesystem::exists(dll_path))
			{
#if 1
				return create_process_use_detour(application, command_line, creation_flags, current_directory, startup_info, process_information, dll_path.string().c_str());
#else
				bool result = create_process_use_system(application, command_line, creation_flags | CREATE_SUSPENDED, current_directory, startup_info, process_information);

				if (result) 
				{
					if (!detail::inject_dll(process_information->hProcess, process_information->hThread, dll_path.c_str()))
					{
						result = false;
					}
					::ResumeThread(pi.hThread);
				}

				return result;
#endif
			}
			else
			{
				return create_process_use_system(application, command_line, creation_flags, current_directory, startup_info, process_information);
			}
		}
	}

	process::process()
		: statue_(PROCESS_STATUE_READY)
	{
		si_.cb = sizeof STARTUPINFOW;
		::GetStartupInfoW(&si_);
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

	bool process::redirect(HANDLE std_input, HANDLE std_output, HANDLE std_error)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			if (std_input || std_output || std_error)
			{
				si_.dwFlags |= STARTF_USESTDHANDLES;

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
				std::unique_ptr<wchar_t[]> command_line_buffer(new wchar_t[command_line.size()+1]);
				wcscpy_s(command_line_buffer.get(), command_line.size()+1, command_line.c_str());

				if (!detail::create_process(
						boost::filesystem::exists(application) ? application.c_str(): nullptr, 
						command_line_buffer.get(), 
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
				std::unique_ptr<wchar_t[]> command_line_buffer(new wchar_t[command_line.size()+1]);
				wcscpy_s(command_line_buffer.get(), command_line.size()+1, command_line.c_str());

				if (!detail::create_process(
					boost::filesystem::exists(application) ? application.c_str(): nullptr, 
					command_line_buffer.get(), 
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
				std::unique_ptr<wchar_t[]> command_line_buffer(new wchar_t[command_line.size()+1]);
				wcscpy_s(command_line_buffer.get(), command_line.size()+1, command_line.c_str());

				if (!detail::create_process(
					nullptr, 
					command_line_buffer.get(), 
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
				std::unique_ptr<wchar_t[]> command_line_buffer(new wchar_t[command_line.size()+1]);
				wcscpy_s(command_line_buffer.get(), command_line.size()+1, command_line.c_str());

				if (!detail::create_process(
					nullptr, 
					command_line_buffer.get(), 
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
			uint32_t code = 0;
			::WaitForSingleObject(pi_.hProcess, INFINITE);
			bool result = !!::GetExitCodeProcess(pi_.hProcess, reinterpret_cast<LPDWORD>(&code));
			return result ? code : 0;
		}

		return 0;
	}

	bool process::close()
	{
		if (statue_ == PROCESS_STATUE_RUNNING)
		{
			statue_ = PROCESS_STATUE_CLOSE;
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
			statue_ = PROCESS_STATUE_CLOSE;
			*pi_ptr = pi_;
			return true;
		}

		return false;
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
