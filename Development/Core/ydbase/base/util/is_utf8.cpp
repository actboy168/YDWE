#include <base/util/unicode.h>

namespace base {

	static const char g_trailing_bytes_for_utf8[] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
	};

	static bool is_legal_utf8(const unsigned char *source, size_t length)
	{
		unsigned char a;
		const unsigned char *srcptr = source;
		switch (length)
		{
		case 6: if (0 == *srcptr++) return false;
		case 5: if (0 == *srcptr++) return false;
		case 4: if (0 == *srcptr++) return false;
		case 3: if (0 == *srcptr++) return false;
		case 2: if (0 == *srcptr++) return false;
		case 1: if (0 == *srcptr++) return false;
		}
		switch (length)
		{
		default: return false;
		case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
		case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
		case 2: if ((a = (*--srcptr)) > 0xBF) return false;
			switch (*source)
			{
			case 0xE0: if (a < 0xA0) return false; break;
			case 0xED: if (a > 0x9F) return false; break;
			case 0xF0: if (a < 0x90) return false; break;
			case 0xF4: if (a > 0x8F) return false; break;
			default:   if (a < 0x80) return false;
			}
		case 1: if (*source >= 0x80 && *source < 0xC2) return false;
		}
		if (*source > 0xF4) return false;
		return true;
	}

	bool is_utf8(const char *source)
	{
		const unsigned char* cur = (const unsigned char*)source;
		while (*cur)
		{
			unsigned short next_n = g_trailing_bytes_for_utf8[*cur] + 1;
			if (!is_legal_utf8(cur, next_n))
			{
				return false;
			}
			cur += next_n;
		}
		return true;
	}

	bool is_utf8(const char *source, size_t length)
	{
		const unsigned char* begin = (const unsigned char*)source;
		const unsigned char* end = begin + length;
		const unsigned char* cur = begin;
		while (cur < end)
		{
			unsigned short next_n = g_trailing_bytes_for_utf8[*cur] + 1;
			if (0 == *cur || cur + next_n > end)
			{
				return false;
			}
			if (!is_legal_utf8(cur, next_n))
			{
				return false;
			}
			cur += next_n;
		}
		return true;
	}
}
