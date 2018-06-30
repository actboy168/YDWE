#pragma once

#include <base/config.h>	   	  		
#include <base/filesystem.h>
#include <Windows.h>

namespace base { namespace path {
	enum PATH_TYPE
	{
		DIR_EXE = 0,
		DIR_MODULE,
		DIR_TEMP,
		DIR_START_MENU,
		DIR_USER_DESKTOP,
		DIR_USER_QUICK_LAUNCH,
		DIR_TASKBAR_PINS,
	};

	_BASE_API fs::path module(HMODULE module_handle);
	_BASE_API fs::path get(PATH_TYPE type);
}}
