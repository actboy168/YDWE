#pragma once

#include <base/config.h>
#include <base/util/noncopyable.h>
#include <Windows.h>

namespace base {

namespace win {

	template <HANDLE NullHandle>
	class scoped_handle 
		: private noncopyable
	{
	public:
		scoped_handle()
			: handle_(NullHandle)
		{ }

		scoped_handle(HANDLE h)
			: handle_(h)
		{ }

		~scoped_handle()
		{
			if (this->operator bool())
			{
				::CloseHandle(handle_);
				handle_ = NullHandle;
			}
		}

		HANDLE get() const
		{
			return handle_;
		}

		operator bool() const
		{
			return NullHandle != handle_;
		}

	protected:
		HANDLE handle_;
	};
}

}
