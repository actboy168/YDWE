#pragma once

#include <base/util/noncopyable.h>

namespace base { namespace lock {

	template <class T>
	class guard
		: private noncopyable
	{
	public:
		inline explicit guard(T& l)
			: lock_(l)
		{
			lock_.lock();
		}

		inline ~guard()
		{ 
			lock_.unlock();
		}

	private:
		T& lock_;

	};
}}
