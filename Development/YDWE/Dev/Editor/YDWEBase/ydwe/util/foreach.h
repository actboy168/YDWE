#pragma once

#if defined(_MSC_VER) && (_MSC_VER > 1500 || (_MSC_VER == 1500 && defined (_HAS_TR1)))
#	if _MSC_VER >= 1700
#		define foreach(VAR, COL) for(VAR : COL)
#	else
#		define foreach(VAR, COL) \
			__pragma(warning(push)) \
			__pragma(warning(disable: 4239)) \
			for each (VAR in COL) \
			__pragma(warning(pop))
#	endif
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#	define foreach(VAR, COL) for(VAR : COL)
#else
#	include <boost/foreach.hpp>
#	define foreach BOOST_FOREACH
#endif
