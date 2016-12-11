#pragma once

#include <base/config.h>
#include <base/util/optional.h>
#include <string>

namespace base { namespace win {

	class _BASE_API env_variable
	{
	public:
		env_variable(const wchar_t* name);
		env_variable(std::wstring const& name);
		env_variable(std::wstring&& name);
		std::wstring                  get();
		std::optional<std::wstring> get_nothrow() throw();
		bool                          set(std::wstring const& value);
		bool                          del();

	private:
#pragma warning(push)
#pragma warning(disable:4251)
		std::wstring               name_;
#pragma warning(pop)
	};
}}
