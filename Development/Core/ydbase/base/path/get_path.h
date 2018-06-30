#pragma once

#include <base/config.h>	   	  		
#include <base/filesystem.h>
#include <Windows.h>

namespace base { namespace path {
	_BASE_API fs::path module(HMODULE module_handle = NULL);
	_BASE_API fs::path temp();
}}
