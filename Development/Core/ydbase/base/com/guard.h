#pragma once

#include <Windows.h>
#include <base/exception/windows_exception.h>

namespace base { namespace com {
	class guard
	{
	public:
		guard() 
			: com_init_(S_OK) 
		{ 
			com_init_ = ::CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

			if (com_init_ != RPC_E_CHANGED_MODE)
			{
				if (FAILED(com_init_))
				{
					throw windows_exception("com initialize failed", com_init_);
				}
			}
		}

		~guard() 
		{
			if (SUCCEEDED(com_init_))
			{
				::CoUninitialize();
			}
		}
	private:
		HRESULT com_init_;
	};
}}
