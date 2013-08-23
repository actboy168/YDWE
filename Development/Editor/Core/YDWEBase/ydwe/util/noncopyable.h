#pragma once

#include <ydwe/config.h>

_BASE_BEGIN 
namespace util {
	class noncopyable
	{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:
		noncopyable( const noncopyable& );
		const noncopyable& operator=( const noncopyable& );
	};
}
_BASE_END
