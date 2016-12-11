#pragma once

#include <string>
#include <base/config.h>
#include <base/util/string_view.h>
#include <cstdint>

namespace base {
	class conv_method
	{
	public:
		enum
		{
			stop            = 0 << 16,
			skip            = 1 << 16,
			replace         = 2 << 16,
		};

		conv_method(uint32_t value)
			: value_(value)
		{ }

		uint32_t type() const 
		{
			return value_ & 0xFFFF0000;
		}

		uint16_t default_char() const 
		{
			return value_ & 0x0000FFFF;
		}

	private:
		uint32_t value_;
	};

	_BASE_API std::wstring u2w(std::string_view  const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  w2u(std::wstring_view const& from, conv_method how = conv_method::stop);

#if !defined(BASE_UNICODE_DISABLE_ANSI)
	_BASE_API std::wstring a2w(std::string_view  const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  w2a(std::wstring_view const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  u2a(std::string_view  const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  a2u(std::string_view  const& from, conv_method how = conv_method::stop);
#endif

	_BASE_API bool is_utf8(const char *source);
	_BASE_API bool is_utf8(const char *source, size_t length);
}
