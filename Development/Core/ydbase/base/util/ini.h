#pragma once

#include <assert.h>
#include <string>
#include <vector>
#include <base/exception/exception.h>
#include <base/util/foreach.h>

namespace base { namespace ini {
	template <typename Table, typename String = Table::key_type>
	struct basic_reader {
		const char* z;
		int line = 1;
		Table& t;
		String sec;
		String key;

		static void expect(const char*& p, const char c)
		{
			assert(*p == c);
			p++;
		}

		static bool equal(const char* p, const char c)
		{
			return *p == c;
		}

		static bool equal(const char* p, const char c[])
		{
			return *p == c[0] || *p == c[1];
		}

		static bool consume(const char*& p, const char c)
		{
			if (equal(p, c)) {
				p++;
				return true;
			}
			return false;
		}

		static bool consume(const char*& p, const char c[])
		{
			if (equal(p, c)) {
				p++;
				return true;
			}
			return false;
		}

		basic_reader(Table& t, const char* input)
			: t(t)
			, z(input)
		{
			if (z[0] == '\xEF' && z[1] == '\xBB' && z[2] == '\xBF') {
				z += 3;
			}
		}

		void incline()
		{
			char old = *z;
			assert(equal(z, "\n\r"));
			z++;
			if (equal(z, "\n\r") && !equal(z, old))
				z++;
			++line;
		}

		void parse_whitespace()
		{
			for (;;)
			{
				if (!consume(z, " \t"))
				{
					return;
				}
			}
		}

		void parse_newline()
		{
			for (;; z++) {
				switch (*z) {
				case '\n':
				case '\r':
				case '\0':
					return;
				}
			}
		}

		template <class ... Args>
		bool error(const char* fmt, const Args& ... args)
		{
			throw exception(fmt, args...);
			return false;
		}

		bool parse_comment()
		{
			expect(z, '/');
			expect(z, '/');
			for (;; z++) {
				switch (*z) {
				case '\n':
				case '\r':
				case '\0':
					return true;
				}
			}
		}

		bool parse_section()
		{
			expect(z, '[');
			const char* p = z;
			for (;; z++) {
				switch (*z) {
				case ']':
					accept_section(p, z - p);
					z++;
					parse_newline();
					return true;
				case '\n':
				case '\r':
				case '\0':
					return error("']' expected near '%c'", *z);
				}
			}
		}

		bool parse_string()
		{
			const char* p = z;
			for (;; z++) {
				switch (*z) {
				case '"':
					accept_value(p, z - p);
					for (;; z++) {
						switch (*z) {
						case '\n':
						case '\r':
						case '\0':
							return true;
						}
					}
					return true;
				case '\n':
				case '\r':
				case '\0':
					accept_value(p, z - p);
					return true;
				}
			}
		}

		bool parse_value()
		{
			if (*z == '"') {
				z++;
				return parse_string();
			}
			const char* p = z;
			for (;; z++) {
				switch (*z) {
				case '\n':
				case '\r':
				case '\0':
					accept_value(p, z - p);
					return true;
				}
			}
		}

		bool parse_assgin()
		{
			const char* p = z;
			for (;; z++) {
				switch (*z) {
				case '=':
					accept_key(p, z - p);
					z++;
					return parse_value();
				case '\n':
				case '\r':
				case '\0':
					return true;
				}
			}
		}

		bool parse_line()
		{
			switch (*z) {
			case '[':
				return parse_section();
			case '/':
				if (z[1] == '/') {
					return parse_comment();
				}
			default:
				return parse_assgin();
			}
		}

		bool parse()
		{
			for (;;) {
				if (!parse_line()) {
					return false;
				}
				switch (*z) {
				case '\0':
					return true;
				case '\n':
				case '\r':
					incline();
					break;
				default:
					return error("'\\n' expected near '%c'", *z);
				}
			}
		}

		void trim(const char*& str, size_t& len)
		{
			const char* end = str + len - 1;
			for (;str < end && isspace((unsigned char)*end); end--)
			{
			}
			for (; str < end && isspace((unsigned char)*str); str++)
			{
			}
			len = end - str + 1;
		}

		void accept_section(const char* str, size_t len)
		{
			trim(str, len);
			sec = String(str, len);
		}

		void accept_key(const char* str, size_t len)
		{
			trim(str, len);
			key = String(str, len);
		}

		void accept_value(const char* str, size_t len)
		{
			trim(str, len);
			t[sec][key] = String(str, len);
		}
	};

	struct strbuilder
		: public std::vector<std::string>
	{
		strbuilder()
			: n(0)
		{ }

		void clear()
		{
			n = 0;
			std::vector<std::string>::clear();
		}

		strbuilder& operator +=	(const std::string& s)
		{
			push_back(s);
			n += s.size();
			return *this;
		}

		strbuilder& operator +=	(std::string&& s)
		{
			push_back(std::forward<std::string>(s));
			n += s.size();
			return *this;
		}

		strbuilder& operator +=	(strbuilder&& sb)
		{
			for (const std::string& s : sb)
			{
				(*this) += std::move(s);
			}
			sb.clear();
			return *this;
		}

		operator std::string()
		{
			std::string r;
			r.reserve(n);
			for (const std::string& s : *this)
			{
				r += std::move(s);
			}
			return std::move(r);
		}

		size_t n;
	};

	typedef std::map<std::string, std::map<std::string, std::string>> table;
	typedef basic_reader<table> reader;

	template <typename T = table>
	void read(T& t, const char* input)
	{
		reader(t, input).parse();
	}

	template <typename T = table>
	std::string write(T const& t)
	{
		strbuilder output;
		foreach(auto const& SectionIt, t)
		{
			std::string const& Section = SectionIt.first;
			output += "[" + Section + "]\n";
			foreach(auto const& KeyValueIt, SectionIt.second)
			{
				std::string const& Key = KeyValueIt.first;
				std::string const& Value = KeyValueIt.second;
				output += Key + " = " + Value + "\n";
			}
		}
		return std::move(std::string(output));
	}
}}
