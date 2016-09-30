/*
 * Logger for injected dll
 */
#ifndef YDWE_LOGGER_H_INCLUDED
#define YDWE_LOGGER_H_INCLUDED

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include <../Plugin/Lua/log/logging.h>

namespace NYDWE
{
	extern logging::logger* lg;
}

#endif // YDWE_LOGGER_H_INCLUDED
