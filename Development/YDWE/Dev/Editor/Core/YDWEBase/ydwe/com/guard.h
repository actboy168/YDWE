#pragma once

#include <Windows.h>
#include <ydwe/exception/windows_exception.h>

_BASE_BEGIN namespace com {
	class guard
	{
	public:
		guard() 
			: com_init_(S_OK) 
		{ 
			com_init_ = ::CoInitialize(NULL);

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
