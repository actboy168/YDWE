#pragma once

#include <base/config.h>		   	  		
#include <base/filesystem.h>   	   	  		
#include <base/util/optional.h>
#include <Windows.h>
#include <map>
#include <set>

namespace base { namespace win {

	namespace ignore_case {
		template <class T> struct less;
		template <> struct less<wchar_t> {
			bool operator()(const wchar_t& lft, const wchar_t& rht) const
			{ return (towlower(static_cast<wint_t>(lft)) < towlower(static_cast<wint_t>(rht))); }
		};
		template <> struct less<std::wstring> {
			bool operator()(const std::wstring& lft, const std::wstring& rht) const 
			{ return std::lexicographical_compare(lft.begin(), lft.end(), rht.begin(), rht.end(), less<wchar_t>()); }
		};
	}

	class _BASE_API process
	{
	public:
		enum STATUE
		{
			PROCESS_STATUE_READY,
			PROCESS_STATUE_RUNNING,
		};

		enum CONSOLE
		{
			CONSOLE_INHERIT,
			CONSOLE_DISABLE,
			CONSOLE_NEW,
		};

		process();
		~process();
		bool     inject_x86(const fs::path& dllpath);
		bool     inject_x64(const fs::path& dllpath);
		bool     replace(const fs::path& dllpath, const char* dllname);
		bool     set_console(CONSOLE type);
		bool     hide_window();
		bool     redirect(HANDLE std_input, HANDLE std_output, HANDLE std_error);
		bool     create(const std::optional<fs::path>& application, const std::wstring& command_line, const std::optional<fs::path>& current_directory = std::optional<fs::path>());
		bool     is_running();
		bool     kill(uint32_t timeout);
		uint32_t wait();
		bool     wait(uint32_t timeout);
		bool     close();
		bool     release(PROCESS_INFORMATION* pi_ptr);
		int      id() const;
		void     set_env(const std::wstring& key, const std::wstring& value);
		void     del_env(const std::wstring& key);

	private:
		uint32_t                statue_;
#pragma warning(push)
#pragma warning(disable:4251)
		fs::path injectdll_x86_;
		fs::path injectdll_x64_;
		std::map<std::string, fs::path> replace_dll_;
		std::map<std::wstring, std::wstring, ignore_case::less<std::wstring>> set_env_;
		std::set<std::wstring, ignore_case::less<std::wstring>>               del_env_;
#pragma warning(pop)
		STARTUPINFOW            si_;
		PROCESS_INFORMATION     pi_;
		bool                    inherit_handle_;
		DWORD                   flags_;
	};
}}
