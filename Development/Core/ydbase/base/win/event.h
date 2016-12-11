#pragma once

#include <base/config.h>
#include <base/exception/windows_exception.h>
#include <base/util/noncopyable.h>
#include <base/win/scoped_handle.h>
#include <cstdint>

namespace base {
namespace win {

	class event 
		: private noncopyable
		, private scoped_handle<NULL>
	{
	public:
		typedef scoped_handle<NULL> mybase;

		event(const wchar_t* name, uint32_t desired_access)
			: mybase(::OpenEventW(desired_access, FALSE, name))
		{
			if (!*this)
			{
				throw windows_exception("Can't open event." );
			}
		}

		event(const wchar_t* name, bool auto_reset)
			: mybase(::CreateEventW(NULL, auto_reset? FALSE : TRUE, FALSE, name))
		{
			if (!*this)
			{
				throw windows_exception("Can't create event." );
			}
		}

		void set()
		{
			if (!::SetEvent(mybase::get()))
			{
				throw windows_exception("Can't signal event." );
			}
		}

		void wait()
		{
			switch (::WaitForSingleObject(mybase::get(), INFINITE))
			{
			case WAIT_OBJECT_0:
				return;
			default:
				throw windows_exception("Wait for event failed." );
			}
		}

		bool wait(long milliseconds)
		{
			switch (::WaitForSingleObject(mybase::get(), milliseconds))
			{
			case WAIT_TIMEOUT:
				return false;
			case WAIT_OBJECT_0:
				return true;
			default:
				throw windows_exception("Wait for event failed." );
			}
		}

		void reset()
		{
			if (!::ResetEvent(mybase::get()))
			{
				throw windows_exception("Can't reset event." );
			}
		}
	};
}
}
