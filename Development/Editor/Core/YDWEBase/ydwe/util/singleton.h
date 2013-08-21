#pragma once

#include <ydwe/util/noncopyable.h>

namespace ydwe { namespace util {
	template <typename object_type>
	struct singleton_nonthreadsafe : public noncopyable
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
