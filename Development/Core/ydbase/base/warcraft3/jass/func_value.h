#pragma once

#include <base/config.h>
#include <cstdint>
#include <vector>
#include <unordered_map>

namespace base { namespace warcraft3 { namespace jass {

	class call_param;

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

	class _BASE_API func_value
	{
	public:
		func_value();
		func_value(const char* param, uintptr_t address);
		func_value(func_value const& that, uintptr_t address);
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

		uintptr_t call(const uintptr_t* param) const;
		uintptr_t call(const call_param& param) const;
		
	private:
		variable_type              return_;
#pragma warning(push)
#pragma warning(disable:4251)
		std::vector<variable_type> param_;
#pragma warning(pop)
		uintptr_t                  address_;
	};

	typedef std::unordered_map<std::string, func_value> func_mapping;
	_BASE_API extern func_mapping jass_function;
	_BASE_API extern func_mapping japi_function;

	_BASE_API func_value const* jass_func(const char* proc_name);
	_BASE_API func_value const* japi_func(const char* proc_name);
	/*_BASE_API*/ bool japi_func_add(const char* proc_name, uintptr_t new_proc);
	/*_BASE_API*/ bool japi_func_add(const char* proc_name, uintptr_t new_proc, const char* param);
	/*_BASE_API*/ bool japi_func_remove(const char* proc_name);
}}}
