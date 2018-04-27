#pragma once

#include <base/config.h>
#include <base/warcraft3/basic_searcher.h>

namespace base { namespace warcraft3 {

	class _BASE_API war3_searcher : public basic_searcher
	{
		typedef basic_searcher _Mybase;
	public:
		war3_searcher();
		war3_searcher(HMODULE hGameDll);

		uint32_t  get_version() const;
		uint32_t  get_instance(uint32_t index);
		uint32_t  get_gameui(uint32_t unk0, uint32_t unk1);
		bool      is_gaming();
		uintptr_t current_function(uintptr_t ptr);

	private:
		uint32_t  search_version() const;
		uintptr_t search_get_instance() const;
		uintptr_t search_get_gameui() const;

	private:
		uint32_t  version_;
		uintptr_t get_instance_;
		uintptr_t get_gameui_;
	};

	struct objectid_64
	{
		uint32_t a;
		uint32_t b;

		inline objectid_64(uint32_t a_, uint32_t b_)
			: a(a_)
			, b(b_)
		{ }

		inline bool is_valid() const
		{
			return (a & b) != (uint32_t)(-1);
		}
	};

	_BASE_API war3_searcher& get_war3_searcher();
	_BASE_API uintptr_t   find_objectid_64(const objectid_64& id);
	_BASE_API const char* get_class_name(uintptr_t ptr);
	_BASE_API uintptr_t   get_vfn_ptr(const char* name);
	_BASE_API uint32_t    get_object_type(uintptr_t ptr);
	_BASE_API uintptr_t   handle_to_object(uint32_t handle);
	_BASE_API uint32_t    object_to_handle(uintptr_t obj);
}}
