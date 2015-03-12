#pragma once

#include <base/config.h>

namespace base { 
	namespace detail {
		struct do_once_flag_nonthreadsafe
		{
			do_once_flag_nonthreadsafe()
				: first_(true)
			{ }

			bool first()
			{
				bool r = first_;
				first_ = false;
				return r;
			}

			bool first_;
		};
	}

#define DO_ONCE_NOTHREADSAFE() static ::base::detail::do_once_flag_nonthreadsafe do_once_flag_ ## __LINE__; if (do_once_flag_ ## __LINE__.first())

}
