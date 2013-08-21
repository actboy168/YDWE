#pragma once

#include <ydwe/config.h>

namespace ydwe { namespace util {
	class YDWE_BASE_API noncopyable
	{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:
		noncopyable( const noncopyable& );
		const noncopyable& operator=( const noncopyable& );
	};
}}
