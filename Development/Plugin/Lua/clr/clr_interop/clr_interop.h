#pragma once

namespace clr {

	class appdomain_impl;
	class appdomain {
	public:
		appdomain(const wchar_t* version);
		~appdomain();
		operator bool() const;
		appdomain_impl* get() const;

	private:
		appdomain_impl* m_impl;
	};

	class object_impl;
	class object {
	public:
		object(appdomain* appdomain, const wchar_t* assembly, const wchar_t* type);
		~object();
		int call(const wchar_t* name);
		int error_code() const;

	private:
		object_impl* m_impl;
	};
}
