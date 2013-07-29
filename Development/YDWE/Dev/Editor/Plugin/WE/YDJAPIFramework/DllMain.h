#ifndef DLLMAIN_H_INCLUDED
#define DLLMAIN_H_INCLUDED

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include <windows.h>
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4231)
#endif
#include <log4cxx/logger.h>
#include <log4cxx/helpers/exception.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace NYDWE
{

extern HMODULE gSelfModule;

extern log4cxx::LoggerPtr gJAPIFrameworkLogger;

} // namespace NYDWE

#endif // DLLMAIN_H_INCLUDED
