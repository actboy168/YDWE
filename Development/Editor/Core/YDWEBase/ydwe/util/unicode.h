#pragma once

#include <string>
#include <ydwe/config.h>
#include <ydwe/util/string_ref.h>
#include <cstdint>

_BASE_BEGIN 
namespace util {
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

	_BASE_API std::wstring u2w(std::string  const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  w2u(std::wstring const& from, conv_method how = conv_method::stop);
	_BASE_API std::wstring a2w(std::string  const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  w2a(std::wstring const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  u2a(std::string  const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  a2u(std::string  const& from, conv_method how = conv_method::stop);

	_BASE_API std::wstring u2w_ref(boost::string_ref  const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  w2u_ref(boost::wstring_ref const& from, conv_method how = conv_method::stop);
	_BASE_API std::wstring a2w_ref(boost::string_ref  const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  w2a_ref(boost::wstring_ref const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  u2a_ref(boost::string_ref  const& from, conv_method how = conv_method::stop);
	_BASE_API std::string  a2u_ref(boost::string_ref  const& from, conv_method how = conv_method::stop);

	_BASE_API bool is_utf8(const char *source);
}
_BASE_END
