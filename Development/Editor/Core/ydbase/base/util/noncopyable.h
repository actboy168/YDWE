#pragma once

#include <base/config.h>

namespace base { 
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
}
