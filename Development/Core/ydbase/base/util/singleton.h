#pragma once

#include <base/util/noncopyable.h>

namespace base {
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

	template <typename object_type>
	struct singleton_threadsafe
		: private noncopyable
	{
	private:
		struct object_creator 
		{
			object_creator() 
			{
				singleton_threadsafe<object_type>::instance();
			}

			inline void do_nothing() const 
			{ }
		};

		static object_creator create_object;
		singleton_threadsafe();

	public:
		static object_type & instance()
		{
			static object_type obj;
			create_object.do_nothing();
			return obj;
		}
	};
	template <typename object_type>
	typename singleton_threadsafe<object_type>::object_creator singleton_threadsafe<object_type>::create_object;
}
