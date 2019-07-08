#pragma once

#include <Windows.h>
#include <bee/error.h>

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
					throw bee::make_error(com_init_, "com initialize failed");
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
