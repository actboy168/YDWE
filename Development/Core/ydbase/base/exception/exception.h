#pragma once

#include <base/config.h>
#include <Windows.h>
#include <stdarg.h>
#include <exception>

namespace base { 

#pragma warning(push)
#pragma warning(disable:4275)
	class _BASE_API exception : public std::exception
	{
	public:
		exception();
		exception(const char* fmt, ...);
		exception(const wchar_t* fmt, ...);
		virtual ~exception();
		virtual const char* what() const;

		template <class CharT>
		class basic_errormsg
		{
		public:
			basic_errormsg() : data(nullptr) { }
			basic_errormsg(size_t n) : data(static_cast<CharT*>(::LocalAlloc(0, n * sizeof(CharT)))) { }
			basic_errormsg(const basic_errormsg& that) : data(nullptr) { *this = that; }
			~basic_errormsg() { reset(0, 0); }
			basic_errormsg& operator =(const basic_errormsg& o) {
				if (this != &o) {
					reset(o.data, ::LocalSize(o.data) / sizeof(CharT));
				}
				return *this;
			}
			void reset(const CharT* buf, size_t len) {
				if (!buf || !len) {
					if (data) {
						::LocalFree(reinterpret_cast<HLOCAL>(data));
					}
					data = nullptr;
					return;
				}
				data = static_cast<CharT*>(::LocalAlloc(0, len * sizeof(CharT)));
				if (data) {
					memcpy(data, buf, len * sizeof(CharT));
				}
			}

			CharT*       c_str() { return data; }
			const CharT* c_str() const { return data; }
			CharT**      get() { return &data; }
			operator bool()      const { return !!data; }

		private:
			CharT* data;
		};

		typedef basic_errormsg<char>    errormsg;
		typedef basic_errormsg<wchar_t> werrormsg;

	protected:
		errormsg  get_format_string(const char* fmt, ...) const;
		errormsg  get_format_string(const char* fmt, va_list argsList) const;
		werrormsg get_format_string(const wchar_t* fmt, ...) const;
		werrormsg get_format_string(const wchar_t* fmt, va_list argsList) const;
#pragma warning(suppress:4251)
		errormsg  what_;
	};
#pragma warning(pop)
}
