#pragma once

#include <ydwe/config.h>
#include <cassert>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <ydwe/util/hybrid_array.h>

#pragma warning(push)
#pragma warning(disable:4702)

_BASE_BEGIN
namespace util { namespace format_detail {

struct default_report_error
{
	default_report_error(const char* reason)
	{
		assert(0 && reason);
		throw std::exception(reason);
	}
};

template <class T>
inline int crt_snprintf(char* buf, size_t buf_size, const char* fmt, const T& value)
{
#pragma warning(suppress:4996)
	return _snprintf(&*buffer_.begin() + len, offset, format_.data(), value);
}

template <class T>
inline int crt_snprintf(wchar_t* buf, size_t buf_size, const wchar_t* fmt, const T& value)
{
#pragma warning(suppress:4996)
	return _snwprintf(buf, buf_size, fmt, value);
}

template <class CharT, class ReportErrorT = default_report_error>
class format_analyzer
{
public:
	typedef CharT char_t;

	enum FLAGS {
		FL_SIGN       = 0x00001,   /* put plus or minus in front */
		FL_SIGNSP     = 0x00002,   /* put space or minus in front */
		FL_LEFT       = 0x00004,   /* left justify */
		FL_LEADZERO   = 0x00008,   /* pad with leading zeros */
		FL_LONG       = 0x00010,   /* long value given */
		FL_SHORT      = 0x00020,   /* short value given */
		FL_SIGNED     = 0x00040,   /* signed data given */
		FL_ALTERNATE  = 0x00080,   /* alternate form requested */
		FL_NEGATIVE   = 0x00100,   /* value is negative */
		FL_FORCEOCTAL = 0x00200,   /* force leading '0' for octals */
		FL_LONGDOUBLE = 0x00400,   /* long double value given */
		FL_WIDECHAR   = 0x00800,   /* wide characters */
		FL_LONGLONG   = 0x01000,   /* long long value given */
		FL_I64        = 0x08000,   /* __int64 value given */
		FL_FORCEHEX   = 0x10000,   /* force leading '0x' for hex */
	};

public:
	format_analyzer(const char_t* fmt)
		: buffer_()
		, format_()
		, temp_()
		, fmt_(fmt)
		, flags_(0)
		, width_(-1)
		, precision_(-1)
		, ch_(0)
	{ }

	~format_analyzer()
	{ }

	void finish()
	{
		fmt_ = print_string_literal(fmt_);
		if (*fmt_ != '\0')
		{
			ReportErrorT("format: Too many conversion specifiers in format string");
		}
	}

	void write(std::basic_ostream<char_t, std::char_traits<char_t>>& out)
	{
		out.write(buffer_.data(), buffer_.size());
	}

	std::basic_string<CharT> str()
	{
		return std::move(std::basic_string<CharT>(buffer_.begin(), buffer_.end()));
	}

	template <class T>
	void accept(const T& value)
	{
		fmt_ = print_string_literal(fmt_);
		fmt_ = print_string_format(fmt_, value);
	}

private:

	void format_value(const char_t* value, std::size_t len)
	{
		std::size_t prefixlen = 0;
		char_t prefix[2];

		if (flags_ & FL_SIGNED)
		{
			if (flags_ & FL_NEGATIVE) 
			{
				prefix[0] = '-';
				prefixlen = 1;
			}
			else if (flags_ & FL_SIGN) 
			{
				prefix[0] = '+';
				prefixlen = 1;
			}
			else if (flags_ & FL_SIGNSP) 
			{
				prefix[0] = ' ';
				prefixlen = 1;
			}
		}
		else if (flags_ & FL_FORCEHEX)
		{
			prefix[0] = '0';
			prefix[1] = 'X' + ch_;
			prefixlen = 2;
		}
		else if (flags_ & FL_FORCEOCTAL)
		{
			prefix[0] = '0';
			prefixlen = 1;
		}

		std::ptrdiff_t padding = width_ - len - prefixlen;

		if (!(flags_ & (FL_LEFT | FL_LEADZERO))) 
		{
			for (std::ptrdiff_t i = 0; i < padding; ++i)
			{
				buffer_.push_back(' ');
			}
		}

		switch (prefixlen)
		{
		case 1:
			buffer_.push_back(prefix[0]);
			break;
		case 2:
			buffer_.push_back(prefix[0]);
			buffer_.push_back(prefix[1]);
			break;
		}

		if ((flags_ & FL_LEADZERO) && !(flags_ & FL_LEFT))
		{
			for (std::ptrdiff_t i = 0; i < padding; ++i)
			{
				buffer_.push_back('0');
			}
		}

		buffer_.append(value, value + len);

		if (flags_ & FL_LEFT) 
		{
			for (std::ptrdiff_t i = 0; i < padding; ++i)
			{
				buffer_.push_back(' ');
			}
		}
	}

	template <class T>
	void format_cast_string(const T& value)
	{
		std::basic_ostringstream<char_t> oss;
		oss << value;
		format_cast_string(oss.str().c_str());
	}

	void format_cast_string(const char_t* value)
	{
		const char_t* first = value;
		const char_t* next = first;

		if (precision_ >= 0)
		{
			while (*next && precision_--) { ++next; }
		}
		else
		{
			while (*next) { ++next; }
		}

		format_value(first, next - first);
	}

	void format_cast_string(char_t* value)
	{
		format_cast_string(const_cast<const char_t*>(value));
	}

	template <size_t n>
	void format_cast_string(const char_t (&value)[n])
	{
		assert(n != 0);

		if (precision_ >= 0)
		{
			format_value(value, (std::min)(static_cast<std::size_t>(precision_), n - 1));
		}
		else
		{
			format_value(value, n - 1);
		}
	}

	template <size_t n>
	void format_cast_string(char_t (&value)[n])
	{
		format_cast_string(const_cast<const char_t*>(value));
	}

	template <class Traits>
	void format_cast_string(const std::basic_string<char_t, Traits>& value)
	{
		format_value(value.c_str(), value.size());
	}

	template <class T>
	void format_cast_char(const T& /*value*/
		, typename std::enable_if<!std::is_convertible<T, char>::value>::type* = 0)
	{
		ReportErrorT("format: Cannot convert from argument type to char.");
	}

	template <class T>
	void format_cast_char(const T& value
		, typename std::enable_if<std::is_convertible<T, char>::value>::type* = 0)
	{
		format_value(reinterpret_cast<const char_t*>(&value), 1);
	}

	template <class T>
	uint64_t convert_to_integer(const T& /*value*/
		, typename std::enable_if<!std::is_convertible<T, uint64_t>::value && !std::is_convertible<T, void*>::value>::type* = 0) 
	{
		ReportErrorT("format: Cannot convert from argument type to integer.");
		return 0;
	}

	template <class T>
	uint64_t convert_to_integer(const T& value
		, typename std::enable_if<std::is_convertible<T, uint64_t>::value>::type* = 0)
	{
		uint64_t number = 0;
		if (flags_ & FL_SIGNED)
		{
			int64_t n = static_cast<int64_t>(value);
			if (n < 0)
			{
				flags_ |= FL_NEGATIVE;
				number = -n;
			}
			else
			{
				number = n;
			}
		}
		else
		{
			number = static_cast<uint64_t>(value);
		}

		return number;
	}

	template <class T>
	uint64_t convert_to_integer(const T& value
		, typename std::enable_if<std::is_convertible<T, void*>::value>::type* = 0)
	{
		return convert_to_integer(reinterpret_cast<uintptr_t>(value));
	}

	template <class T>
	double convert_to_float(const T& /*value*/
		, typename std::enable_if<!std::is_floating_point<T>::value>::type* = 0)
	{
		ReportErrorT("format: Cannot convert from argument type to float.");
		return 0.;
	}

	template <class T>
	double convert_to_float(const T& value
		, typename std::enable_if<std::is_floating_point<T>::value>::type* = 0)
	{
		return static_cast<double>(value);
	}

	template <size_t Base>
	unsigned count_digits(uint64_t n) 
	{
		unsigned count = 1;
		for (;;) 
		{
			if (n < Base) return count;
			if (n < Base*Base) return count + 1;
			if (n < Base*Base*Base) return count + 2;
			if (n < Base*Base*Base*Base) return count + 3;
			n /= (Base*Base*Base*Base);
			count += 4;
		}
	}

	template <size_t Base>
	void format_cast_integer(uint64_t number) 
	{
		unsigned len = count_digits<Base>(number);
		const char* digits = ch_ ? "0123456789abcdef" : "0123456789ABCDEF";

		temp_.resize(len);
		while (number >= Base) 
		{
			unsigned index = number % Base;
			number /= Base;
			temp_[--len] = static_cast<char_t>(digits[index]);
		}
		temp_[0] = static_cast<char_t>(digits[number]);

		format_value(temp_.data(), temp_.size());
	}

	void format_cast_float(const double& value)
	{
		for (std::size_t len = buffer_.size(), offset = 16;; offset *= 2)
		{
			buffer_.resize(len + offset);

			int ret = crt_snprintf(&*buffer_.begin() + len, offset, format_.data(), value);

			if (ret > 0)
			{
				buffer_.resize(len + ret);
				break;
			}
		}
	}

	int parse_int_and_advance(const char_t*& c)
	{
		int i = 0;
		for (;*c >= '0' && *c <= '9'; ++c)
			i = 10 * i + (*c - '0');
		return i;
	}

	void reset_format(const char_t* first, const char_t* last)
	{
		format_.clear();
		format_.append(first, last);
		format_.push_back(0);
	}

	const char_t* print_string_literal(const char_t* fmt)
	{
		const char_t* c = fmt;
		for(;; ++c)
		{
			switch (*c)
			{
			case '\0':
				buffer_.append(fmt, c);
				return c;
			case '%':
				buffer_.append(fmt, c);
				if(*(c+1) != '%')
					return c;
				// for "%%", tack trailing % onto next literal section.
				fmt = ++c;
				break;
			}
		}
	}

	template <class T>
	const char_t* print_string_format(const char_t* fmtStart, const T& value)
	{
		if (*fmtStart != '%')
		{
			ReportErrorT("format: Not enough conversion specifiers in format string");
			return fmtStart;
		}

		flags_ = 0;
		width_ = -1;
		precision_ = -1;
		ch_ = 0;

		const char_t* c = fmtStart + 1;
		// 1) Parse flags
		for(;; ++c)
		{
			switch(*c)
			{
			case '-':
				flags_ |= FL_LEFT;   /* '-' => left justify */
				continue;
			case '+':
				flags_ |= FL_SIGN;   /* '+' => force sign indicator */
				continue;
			case ' ':
				flags_ |= FL_SIGNSP; /* ' ' => force sign or space */
				continue;
			case '#':
				flags_ |= FL_ALTERNATE;  /* '#' => alternate form */
				continue;
			case '0':
				flags_ |= FL_LEADZERO;   /* '0' => pad with leading zeros */
				continue;
			}
			break;
		}
		// 2) Parse width
		if (*c >= '0' && *c <= '9')
		{
			width_ = parse_int_and_advance(c);
		}

		if (*c == '*')
		{
			++c;
			ReportErrorT("format: * conversion spec not supported");
		}

		// 3) Parse precision
		if (*c == '.')
		{
			++c;
			int precision = 0;
			if (*c == '*')
			{
				++c;
				ReportErrorT("format: * conversion spec not supported");
			}
			else
			{
				if (*c >= '0' && *c <= '9')
					precision = parse_int_and_advance(c);
				else if(*c == '-')
					parse_int_and_advance(++c);
			}
			precision_ = precision;
		}

		// 4) Ignore any C99 length modifier
		while (*c == 'l' || *c == 'h' || *c == 'L'
			|| *c == 'j' || *c == 'z' || *c == 't')
		{
			++c;
		}

		// 5) We're up to the conversion specifier character.
		// Set stream flags based on conversion specifier (thanks to the
		// boost::format class for forging the way here).
		if (*c == '\0')
		{
			ReportErrorT("format: Conversion spec incorrectly terminated by end of string");
			return c;
		}

		reset_format(fmtStart, c + 1);

		switch (*c)
		{
		case 'd': case 'i':
			flags_ |= FL_SIGNED;
		case 'u': 
			format_cast_integer<10>(convert_to_integer(value));
			break;
		case 'o':
			if (flags_ & FL_ALTERNATE)
			{
				flags_ |= FL_FORCEOCTAL;
			}
			format_cast_integer<8>(convert_to_integer(value));
			break;
		case 'x':
			ch_ = 'a' - 'A';
		case 'X':
			goto COMMON_HEX;
		case 'p':
			precision_ = 2 * sizeof(uintptr_t);
		COMMON_HEX:
			if (flags_ & FL_ALTERNATE) 
			{
				flags_ |= FL_FORCEHEX;
			}
			format_cast_integer<16>(convert_to_integer(value));
			break;
		case 'A': case 'E': case 'F': case 'G':
		case 'a': case 'e': case 'f': case 'g':
			format_cast_float(convert_to_float(value));
			break;
		case 'c':
			format_cast_char(value);
			break;
		case 's':
			format_cast_string(value);
			break;
		case 'n':
			ReportErrorT("format: %n conversion spec not supported");
			break;
		}

		return c+1;
	}

	hybrid_array<char_t, 512> buffer_;
	hybrid_array<char_t, 16>  format_;
	hybrid_array<char_t, 16>  temp_;
	const char_t*             fmt_;
	std::size_t               flags_;
	std::ptrdiff_t            width_;
	std::ptrdiff_t            precision_;
	char_t                    ch_;
};

std::ostream& standard_output(const char*) { return std::cout; }
std::wostream& standard_output(const wchar_t*) { return std::wcout; }
}

#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>

#define DEFINE_FORMAT_ACCEPT(z, n, param) fmt_iter.accept(BOOST_PP_CAT(param, n));
#define DEFINE_FORMAT_CREATER(z, n, unused) \
	template <class CharT BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, class T)> \
	void format(std::basic_ostream<CharT, std::char_traits<CharT>>& out, const CharT* fmt BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& v)) \
	{ \
		format_detail::format_analyzer<CharT> fmt_iter(fmt); \
		BOOST_PP_REPEAT(n, DEFINE_FORMAT_ACCEPT, v) \
		fmt_iter.finish(); \
		fmt_iter.write(out); \
	} \
	template <class CharT BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, class T)> \
	std::basic_string<CharT> format(const CharT* fmt BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& v)) \
	{ \
		format_detail::format_analyzer<CharT> fmt_iter(fmt); \
		BOOST_PP_REPEAT(n, DEFINE_FORMAT_ACCEPT, v) \
		fmt_iter.finish(); \
		return std::move(fmt_iter.str()); \
	} \
	template <class CharT BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, class T)> \
	void printf(const CharT* fmt BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_BINARY_PARAMS(n, T, const& v)) \
	{ \
		format(format_detail::standard_output(fmt), fmt BOOST_PP_COMMA_IF(n) BOOST_PP_ENUM_PARAMS(n, v)); \
	}

BOOST_PP_REPEAT(16, DEFINE_FORMAT_CREATER, ~)
#undef DEFINE_FORMAT_ACCEPT
#undef DEFINE_FORMAT_CREATER

}

#pragma warning(pop)

_BASE_END
