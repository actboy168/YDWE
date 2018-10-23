#pragma once

#include <base/config.h>

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable:4251)
#include <base/subprocess/subprocess_win.h>
#pragma warning(pop)
namespace base { namespace subprocess = win::subprocess; }
#else
#include <base/subprocess/subprocess_posix.h>
namespace base { namespace subprocess = posix::subprocess; }
#endif
