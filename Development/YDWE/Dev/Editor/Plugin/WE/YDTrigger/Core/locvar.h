#pragma once

#include <Windows.h>

namespace locvar
{
	class guard
	{
	public:
		guard(int id);
		~guard();
		void set_handle(const char* handle_name);

	private:
		int old_;
	};

	void get              (DWORD This, DWORD OutClass, char* type_name);
	void set              (DWORD This, DWORD OutClass, char* name);
	void flush_in_timer   (DWORD This, DWORD OutClass);
	void flush_in_trigger (DWORD This, DWORD OutClass);
	void sleep_after      (DWORD This, DWORD OutClass);
	void return_before    (DWORD This, DWORD OutClass);
	void construct        (DWORD OutClass);
	void destroy          (DWORD OutClass);
}
