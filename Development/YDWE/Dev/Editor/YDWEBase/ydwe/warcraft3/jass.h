#pragma once

#include <ydwe/config.h>
#include <cstdint>

namespace ydwe { namespace warcraft3 { namespace jass {

	typedef void     jnothing_t;
	typedef uint32_t jboolean_t;
	typedef uint32_t jcode_t;
	typedef uint32_t jhandle_t;
	typedef uint32_t jinteger_t;
	typedef uint32_t jreal_t;
	typedef uint32_t jstring_t;

	class YDWE_BASE_API string_fake
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

	YDWE_BASE_API float       from_real      (jreal_t val);
	YDWE_BASE_API jreal_t     to_real        (float val);
	YDWE_BASE_API const char* from_string    (jstring_t val);
	YDWE_BASE_API string_fake to_string      (const char* val);
	YDWE_BASE_API uintptr_t   create_string  (const char* val);
	YDWE_BASE_API const char* from_trigstring(const char* val);

	YDWE_BASE_API uintptr_t   call           (const char* name, ...);
}}}
