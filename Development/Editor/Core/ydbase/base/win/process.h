#pragma once

#include <base/config.h>
#include <boost/filesystem.hpp>
#include <Windows.h>
#include <map>

namespace base { namespace win {

	class _BASE_API process
	{
	public:
		enum STATUE
		{
			PROCESS_STATUE_READY,
			PROCESS_STATUE_RUNNING,
		};
		
		process();
		~process();
		bool     inject(const boost::filesystem::path& dllpath);
		bool     replace(const boost::filesystem::path& dllpath, const char* dllname);
		bool     hide_window();
		bool     redirect(HANDLE std_input, HANDLE std_output, HANDLE std_error);
		bool     create(const boost::filesystem::path& application, const std::wstring& command_line, const boost::filesystem::path& current_directory);
		bool     create(const boost::filesystem::path& application, const std::wstring& command_line);
		bool     create(const std::wstring& command_line, const boost::filesystem::path& current_directory);
		bool     create(const std::wstring& command_line);
		bool     is_running();
		bool     kill(uint32_t timeout);
		uint32_t wait();
		bool     wait(uint32_t timeout);
		bool     close();
		bool     release(PROCESS_INFORMATION* pi_ptr);
		int      id() const;

	private:
		uint32_t                statue_;
		boost::filesystem::path inject_dll_;
#pragma warning(push)
#pragma warning(disable:4251)
		std::map<std::string, boost::filesystem::path> replace_dll_;
#pragma warning(pop)
		STARTUPINFOW            si_;
		PROCESS_INFORMATION     pi_;
		bool                    inherit_handle_;
	};

	_BASE_API bool create_process(
		const boost::filesystem::path& application, 
		const std::wstring&            command_line, 
		const boost::filesystem::path& current_directory, 
		const boost::filesystem::path& inject_dll, 
		PROCESS_INFORMATION*           pi_ptr = nullptr
	);
}}
