#pragma once

#include <base/config.h>	   	  		
#include <base/filesystem.h>
#include <Windows.h>

namespace base { namespace path {
	enum PATH_TYPE
	{
		DIR_EXE = 0,
		DIR_TEMP,
	};

	_BASE_API fs::path module(HMODULE module_handle = NULL);
	_BASE_API fs::path get(PATH_TYPE type);
	_BASE_API fs::path temp(PATH_TYPE type);
}}
