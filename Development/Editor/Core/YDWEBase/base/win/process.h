#pragma once

#include <base/config.h>
#include <boost/filesystem.hpp>
#include <Windows.h>

_BASE_BEGIN

namespace win {

	class _BASE_API process
	{
	public:
		enum STATUE
		{
			PROCESS_STATUE_READY,
			PROCESS_STATUE_RUNNING,
			PROCESS_STATUE_CLOSE,
		};
		
		process();
		~process();
		bool     inject(const boost::filesystem::path& dll_path);
		bool     redirect(HANDLE std_input, HANDLE std_output, HANDLE std_error);
		bool     create(const boost::filesystem::path& application, const std::wstring& command_line, const boost::filesystem::path& current_directory);
		bool     create(const boost::filesystem::path& application, const std::wstring& command_line);
		bool     create(const std::wstring& command_line, const boost::filesystem::path& current_directory);
		bool     create(const std::wstring& command_line);
		uint32_t wait();
		bool     close();
		bool     release(PROCESS_INFORMATION* pi_ptr);

	private:
		uint32_t                statue_;
		boost::filesystem::path inject_dll_;
		STARTUPINFOW            si_;
		PROCESS_INFORMATION     pi_;
	};

	_BASE_API bool create_process(
		const boost::filesystem::path& application, 
		const std::wstring&            command_line, 
		const boost::filesystem::path& current_directory, 
		const boost::filesystem::path& inject_dll, 
		PROCESS_INFORMATION*           pi_ptr = nullptr
	);
}

_BASE_END
