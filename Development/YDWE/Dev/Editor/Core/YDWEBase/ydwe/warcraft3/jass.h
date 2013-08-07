#pragma once

#include <ydwe/config.h>
#include <cstdint>

_BASE_BEGIN namespace warcraft3 { namespace jass {

	typedef void     jnothing_t;
	typedef uint32_t jboolean_t;
	typedef uint32_t jcode_t;
	typedef uint32_t jhandle_t;
	typedef uint32_t jinteger_t;
	typedef uint32_t jreal_t;
	typedef uint32_t jstring_t;

	class _BASE_API string_fake
	{
	public:
		string_fake();
		string_fake(const char* str);
		string_fake(const string_fake& that);
		~string_fake();
		operator jstring_t () const;

	private:
		uint32_t memory_[8];
	};

	_BASE_API float       from_real      (jreal_t val);
	_BASE_API jreal_t     to_real        (float val);
	_BASE_API const char* from_string    (jstring_t val);
	_BASE_API string_fake to_string      (const char* val);
	_BASE_API uintptr_t   create_string  (const char* val);
	_BASE_API const char* from_trigstring(const char* val);

	_BASE_API uintptr_t   call           (const char* name, ...);
	_BASE_API uintptr_t   call           (uintptr_t func_address, uintptr_t param_list[], size_t param_list_size);
}}}
