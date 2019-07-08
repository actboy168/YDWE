#pragma once

#include <assert.h> 

namespace lng {
	struct lex {
		const char* z;
		int line = 1;

		lex(const char* input)
			: z(input)
		{
			if (z[0] == '\xEF' && z[1] == '\xBB' && z[2] == '\xBF') {
				z += 3;
			}
		}

		void incline()
		{
			char old = *z;
			assert(*z == '\n' || *z == '\r');
			z++;
			if ((*z == '\n' || *z == '\r') && *z != old)
				z++;
			++line;
		}

		void parse_whitespace()
		{
			for (;;)
			{
				switch (*z) {
				case '\t':
				case ' ':
					z++;
					break;
				case '\n':
				case '\r':
					incline();
					break;
				default:
					return;
				}
			}
		}

		void parse_whitespace_and_comments()
		{
			parse_whitespace();
			for (; z[0] == '-' && z[1] == '-';) {
				for (z += 2; *z != '\n' && *z != '\r' && *z != '\0'; z++);
				parse_whitespace();
			}
		}

		template <class Handler, class ... Args>
		bool error(Handler& h, const char* fmt, const Args& ... args)
		{
			h.accept_error(line, fmt, args...);
			return false;
		}

		size_t trim(const char* str, size_t len)
		{
			for (const char* p = str + len - 1; p > str; --p) {
				switch (*p) {
				case '\t':
				case ' ':
				case '\n':
				case '\r':
					break;
				default:
					return p - str + 1;
				}
			}
			return 0;
		}

		template <class Handler>
		bool parse_value(Handler& h)
		{
			const char* p = z;
			if (*z == '[') {
				h.accept_value("", 0);
				return true;
			}
			for (;;) {
				switch (*z) {
				case '\n':
				case '\r':
					incline();
					if (*z == '[') {
						h.accept_value(p, trim(p, z - p));
						return true;
					}
					break;
				case '\0':
					h.accept_value(p, trim(p, z - p));
					return true;
				default:
					z++;
					break;
				}
			}
		}

		template <class Handler>
		bool parse_key(Handler& h)
		{
			assert(*z == '[');
			int del = 1;
			const char* p = ++z;
			for (;; z++) {
				switch (*z) {
				case '[':
					del++;
					break;
				case ']':
					if (del) del--;
					if (!del) {
						h.accept_key(p, z - p);
						z++;
						parse_whitespace_and_comments();
						return true;
					}
					break;
				case '\0':
					return error(h, "']' expected near 'eof'");
				default:
					break;
				}
			}
		}

		template <class Handler>
		bool parse_keyvalue(Handler& h)
		{
			if (*z != '[') {
				return error(h, "'[' expected near '%c'", *z);
			}
			if (!parse_key(h)) {
				return false;
			}
			parse_whitespace_and_comments();
			while (*z != '[' && *z != '\0') {
				if (!parse_value(h)) {
					return false;
				}
				parse_whitespace_and_comments();
			}
			return true;
		}

		template <class Handler>
		bool parse(Handler& h)
		{
			parse_whitespace_and_comments();
			while (*z != '\0') {
				if (!parse_keyvalue(h)) {
					return false;
				}
				parse_whitespace_and_comments();
			}
			return true;
		}
	};
}
