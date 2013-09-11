#pragma once

#include <base/util/noncopyable.h>

_BASE_BEGIN namespace util {
	template <typename object_type>
	struct singleton_nonthreadsafe 
		: private noncopyable
	{
	private:
		singleton_nonthreadsafe();

	public:
		static object_type& instance() 
		{
			static object_type obj;
			return obj;
		}
	};
}}
