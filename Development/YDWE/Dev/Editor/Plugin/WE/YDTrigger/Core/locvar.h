#pragma once

#include <Windows.h>
#include <cstdint>

namespace locvar
{
	struct state
	{
		state()
			: mother_id(0)
			, name(nullptr)
			, handle_string(nullptr)
		{ }

		state(uint32_t m, const char* n, const char* h)
			: mother_id(m)
			, name(n)
			, handle_string(h)
		{ }

		uint32_t    mother_id;
		const char* name;
		const char* handle_string;
	};

	class guard
	{
	public:
		guard(int id, const char* name, const char* handle_string);
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
	bool trigger_data     (DWORD This, DWORD OutClass, const char* name);
}
