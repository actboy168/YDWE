/*
 * Logger for injected dll
 */
#ifndef YDWE_LOGGER_H_INCLUDED
#define YDWE_LOGGER_H_INCLUDED

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4231)
#endif
#include <log4cxx/logger.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/helpers/exception.h>
#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace NYDWE
{
	extern log4cxx::LoggerPtr gInjectLogger;
}

#endif // YDWE_LOGGER_H_INCLUDED
