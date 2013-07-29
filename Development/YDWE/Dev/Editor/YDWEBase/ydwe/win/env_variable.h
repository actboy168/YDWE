#pragma once

#include <ydwe/config.h>
#include <string>

namespace ydwe { namespace win {

	class YDWE_BASE_API env_variable
	{
	public:
		env_variable(const wchar_t* name);
		env_variable(std::wstring const& name);
		env_variable(std::wstring&& name);
		std::wstring get();
		bool set(std::wstring const& value);
		bool del();

	private:
#pragma warning(push)
#pragma warning(disable:4251)
		std::wstring               name_;
#pragma warning(pop)
	};
}}
