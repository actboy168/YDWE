#pragma once

#include <base/config.h>
#include <Windows.h>

namespace base { namespace console {

	struct read_req_t
	{
		OVERLAPPED overlapped;
		HANDLE     handle;
		wchar_t    buffer[8192];
	};

	_BASE_API void enable();
	_BASE_API void disable();
	_BASE_API bool disable_close_button();
	_BASE_API void pause();
	_BASE_API bool read_post();
	_BASE_API bool read_try(read_req_t*& req);
	_BASE_API void read_release(read_req_t* req);
}}
