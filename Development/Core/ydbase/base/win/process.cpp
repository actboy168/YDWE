#include <base/win/process.h>
#include <base/hook/injectdll.h>   
#include <base/hook/replacedll.h>
#include <base/util/dynarray.h>
#include <Windows.h>
#include <memory>
#include <deque>
#include <strsafe.h>
#include <cassert>
#if !defined(DISABLE_DETOURS)
#include <detours.h>	  
#pragma comment(lib, "detours.lib")
#endif

namespace base { namespace win {

	namespace detail 
	{
		bool create_process(
			const wchar_t*                 application, 
			wchar_t*                       command_line,
			bool                           inherit_handle,
			uint32_t                       creation_flags,
			wchar_t*                       environment,
			const wchar_t*                 current_directory,
			LPSTARTUPINFOW                 startup_info,
			LPPROCESS_INFORMATION          process_information,
			const fs::path&                injectdll_x86,
			const fs::path&                injectdll_x64,
			const std::map<std::string, fs::path>& replacedll
		)
		{
			bool pause = !replacedll.empty();
			bool suc = false;
			if (fs::exists(injectdll_x86) || fs::exists(injectdll_x64))
			{
				pause = true;
#if !defined(DISABLE_DETOURS)
				suc = !!DetourCreateProcessWithDllW(
					application, command_line,
					NULL, NULL,
					inherit_handle, creation_flags | CREATE_SUSPENDED,
					NULL, current_directory,
					startup_info, process_information,
					injectdll_x86.string().c_str(), NULL
				);
#else
				suc = !!::CreateProcessW(
					application, command_line,
					NULL, NULL,
					inherit_handle, creation_flags | CREATE_SUSPENDED, 
					environment, current_directory,
					startup_info, process_information
				);
#if !defined(_M_X64)
				if (suc)
				{
					hook::injectdll(*process_information, injectdll_x86, injectdll_x64);
				}
#endif
#endif
			}
			else
			{
				suc = !!::CreateProcessW(
					application, command_line, 
					NULL, NULL, 
					inherit_handle, pause ? (creation_flags | CREATE_SUSPENDED) : creation_flags,
					environment, current_directory,
					startup_info, process_information
				);
			}

			if (suc && !replacedll.empty())
			{
				for (auto it = replacedll.begin(); it != replacedll.end(); ++it)
				{
					hook::replacedll(process_information->hProcess, it->first.c_str(), it->second.string().c_str());
				}
			}

			if (suc && pause)
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
				return !wait(process, 0);
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

	struct strbuilder {
		struct node {
			size_t size;
			size_t maxsize;
			wchar_t* data;
			node(size_t maxsize)
				: size(0)
				, maxsize(maxsize)
				, data(new wchar_t[maxsize])
			{ }
			~node() {
				delete[] data;
			}
			wchar_t* release() {
				wchar_t* r = data;
				data = nullptr;
				return r;
			}
			bool append(const wchar_t* str, size_t n) {
				if (size + n > maxsize) {
					return false;
				}
				memcpy(data + size, str, n * sizeof(wchar_t));
				size += n;
				return true;
			}
			template <class T, size_t n>
			void operator +=(T(&str)[n]) {
				append(str, n - 1);
			}
			template <size_t n>
			void operator +=(const strbuilder& str) {
				append(str.data, str.size);
			}
		};
		strbuilder() : size(0) { }
		void clear() {
			size = 0;
			data.clear();
		}
		bool append(const wchar_t* str, size_t n) {
			if (!data.empty() && data.back().append(str, n)) {
				size += n;
				return true;
			}
			size_t m = 1024;
			while (m < n) {
				m *= 2;
			}
			data.emplace_back(m).append(str, n);
			size += n;
			return true;
		}
		template <class T, size_t n>
		strbuilder& operator +=(T(&str)[n]) {
			append(str, n - 1);
			return *this;
		}
		strbuilder& operator +=(const std::wstring& s) {
			append(s.data(), s.size());
			return *this;
		}
		wchar_t* string() {
			node r(size + 1);
			for (auto& s : data) {
				r.append(s.data, s.size);
			}
			r += L"\0";
			return r.release();
		}
		std::deque<node> data;
		size_t size;
	};

	static wchar_t* make_env(std::map<std::wstring, std::wstring, ignore_case::less<std::wstring>>& set, std::set<std::wstring, ignore_case::less<std::wstring>>& del)
	{
		strbuilder res;

		wchar_t* es = GetEnvironmentStringsW();
		if (es == 0) {
			return nullptr;
		}
		try {
			wchar_t* escp = es;
			while (*escp != L'\0') {
				std::wstring str = escp;
				std::wstring::size_type pos = str.find(L'=');
				std::wstring key = str.substr(0, pos);
				if (del.find(key) != del.end()) {
					continue;
				}
				std::wstring val = str.substr(pos + 1, str.length());
				auto it = set.find(key);
				if (it != set.end()) {
					val = it->second;
					set.erase(it);
				}
				res += key;
				res += L"=";
				res += val;
				res += L"\0";

				escp += str.length() + 1;
			}
			for (auto& e : set) {
				const std::wstring& key = e.first;
				const std::wstring& val = e.second;
				if (del.find(key) != del.end()) {
					continue;
				}
				res += key;
				res += L"=";
				res += val;
				res += L"\0";
			}
			return res.string();
		}
		catch (...) {
		}
		FreeEnvironmentStringsW(es);
		return nullptr;
	}

	process::process()
		: statue_(PROCESS_STATUE_READY)
		, inherit_handle_(false)
		, flags_(0)
	{
		memset(&si_, 0, sizeof STARTUPINFOW);
		memset(&pi_, 0, sizeof PROCESS_INFORMATION);
		si_.cb = sizeof STARTUPINFOW;
		si_.dwFlags = 0;
		si_.hStdInput = INVALID_HANDLE_VALUE;
		si_.hStdOutput = INVALID_HANDLE_VALUE;
		si_.hStdError = INVALID_HANDLE_VALUE;
	}

	process::~process()
	{
		close();
	}

	bool process::inject_x86(const fs::path& dllpath)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			injectdll_x86_ = dllpath;
			return true;
		}

		return false;
	}

	bool process::inject_x64(const fs::path& dllpath)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			injectdll_x64_ = dllpath;
			return true;
		}

		return false;
	}

	bool process::replace(const fs::path& dllpath, const char* dllname)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			replacedll_[dllname] = dllpath;
			return true;
		}
		return false;
	}

	bool process::set_console(CONSOLE type)
	{
		if (statue_ == PROCESS_STATUE_READY)
		{
			switch (type) {
			case CONSOLE_INHERIT:
				flags_ = 0;
				break;
			case CONSOLE_DISABLE:
				flags_ = CREATE_NO_WINDOW;
				break;
			case CONSOLE_NEW:
				flags_ = CREATE_NEW_CONSOLE;
				break;
			}
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
			std::unique_ptr<wchar_t[]> environment;
			if (!set_env_.empty() || !del_env_.empty()) {
				environment.reset(make_env(set_env_, del_env_));
				flags_ |= CREATE_UNICODE_ENVIRONMENT;
			}
			std::dynarray<wchar_t> command_line_buffer(command_line.size() + 1);
			wcscpy_s(command_line_buffer.data(), command_line_buffer.size(), command_line.c_str());

			if (!detail::create_process(
				application? application->c_str(): nullptr,
				command_line_buffer.data(),
				inherit_handle_,
				flags_ | NORMAL_PRIORITY_CLASS,
				environment.get(),
				current_directory ? current_directory->c_str() : nullptr,
				&si_, &pi_, injectdll_x86_, injectdll_x64_, replacedll_
				))
			{
				return false;
			}
			statue_ = PROCESS_STATUE_RUNNING;
			::CloseHandle(si_.hStdInput);
			::CloseHandle(si_.hStdOutput);
			::CloseHandle(si_.hStdError);
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

	void process::set_env(const std::wstring& key, const std::wstring& value)
	{
		set_env_[key] = value;
	}

	void process::del_env(const std::wstring& key)
	{
		del_env_.insert(key);
	}
}}
