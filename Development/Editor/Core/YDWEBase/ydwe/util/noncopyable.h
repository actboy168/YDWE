#pragma once

#include <ydwe/config.h>

_BASE_BEGIN namespace util {
	class _BASE_API noncopyable
	{
	protected:
		noncopyable() {}
		~noncopyable() {}
	private:
		noncopyable( const noncopyable& );
		const noncopyable& operator=( const noncopyable& );
	};
}}
