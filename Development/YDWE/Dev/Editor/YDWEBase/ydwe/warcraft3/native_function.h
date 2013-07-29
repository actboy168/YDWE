#pragma once

#include <ydwe/config.h>
#include <cstdint>
#include <vector>

namespace ydwe { namespace warcraft3 { namespace native_function {

	enum variable_type
	{
		TYPE_NONE    = 0,
		TYPE_BOOLEAN = 'B',
		TYPE_CODE    = 'C',
		TYPE_HANDLE  = 'H',
		TYPE_INTEGER = 'I',
		TYPE_REAL    = 'R',
		TYPE_STRING  = 'S',
		TYPE_NOTHING = 'V',
	};

	class YDWE_BASE_API native_function
	{
	public:
		native_function();
		native_function(const char* param, uintptr_t address);
		native_function(native_function const& that, uintptr_t address);
		bool                              is_valid()    const;
		std::vector<variable_type> const& get_param()   const;
		variable_type const&              get_return()  const;
		uintptr_t                         get_address() const;

		template <class R>
		void call_assert(size_t param_count) const
		{
			assert(is_valid());
			assert((return_ != 'V') || ((return_ == 'V') && (std::is_void<R>::value)));
			assert(param_.size() == param_count);
		}

		uintptr_t call(uintptr_t param_list[]) const;

	private:
		variable_type              return_;
#pragma warning(push)
#pragma warning(disable:4251)
		std::vector<variable_type> param_;
#pragma warning(pop)
		uintptr_t                  address_;
	};

	YDWE_BASE_API bool table_hook     (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	YDWE_BASE_API bool table_unhook   (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	YDWE_BASE_API bool register_hook  (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	YDWE_BASE_API bool register_unhook(const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	YDWE_BASE_API bool hook           (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	YDWE_BASE_API bool unhook         (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	YDWE_BASE_API void async_add      (uintptr_t func, const char* name, const char* param);
	YDWE_BASE_API void async_hook     (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	YDWE_BASE_API void japi_add       (uintptr_t func, const char* name, const char* param);
	YDWE_BASE_API void japi_hook      (const char* proc_name, uintptr_t* old_proc_ptr, uintptr_t new_proc);
	YDWE_BASE_API native_function const* jass_func(const char* proc_name);
	YDWE_BASE_API native_function const* japi_func(const char* proc_name);
}}}
