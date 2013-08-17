#pragma once

#include <Windows.h>

namespace locvar
{
	struct state
	{
		state()
			: mother_id(0)
			, handle_string(nullptr)
		{ }

		int         mother_id;
		const char* name;
		const char* handle_string;
	};

	class guard
	{
	public:
		guard(const char* name, int id);
		~guard();
		state& current();

	private:
		state old_;
	};

	void get              (DWORD This, DWORD OutClass, char* type_name);
	void set              (DWORD This, DWORD OutClass, char* name);
	void flush_in_timer   (DWORD This, DWORD OutClass);
	void flush_in_trigger (DWORD This, DWORD OutClass);
	void sleep_after      (DWORD This, DWORD OutClass);
	void return_before    (DWORD This, DWORD OutClass);
	void construct        (DWORD OutClass);
	void destroy          (DWORD OutClass);
	void params           (DWORD This, DWORD OutClass, char* name, DWORD index, char* handle_string);
}
