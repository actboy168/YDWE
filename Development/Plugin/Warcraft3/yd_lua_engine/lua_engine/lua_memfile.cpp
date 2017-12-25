#include "lua_memfile.h"

#include <ctype.h>
#include <locale.h>
#include <lua.hpp>
#include <algorithm>

#define MEMFILE_HANDLE "memfile*"

namespace memfile {

	static int pushresult(lua_State* L, int en) {
		if (en) {
			char buf[256] = { 0 };
			strerror_s(buf, en);
			lua_pushnil(L);
			lua_pushstring(L, buf);
			lua_pushinteger(L, en);
			return 3;
		}
		lua_pushboolean(L, 1);
		return 1;
	}

	struct memfile {
		static memfile* create(lua_State *L, const char* fname, IFileCreate icreate, IFileClose iclose) {
			memfile* self = (memfile *)lua_newuserdata(L, sizeof(memfile));
			new (self) memfile;
			luaL_setmetatable(L, MEMFILE_HANDLE);
			self->m_fclose = iclose;
			int en = icreate(fname, &self->m_data, &self->m_size);
			if (en) {
				char buf[256] = { 0 };
				strerror_s(buf, en);
				lua_pushnil(L);
				lua_pushfstring(L, "%s: %s", fname, buf);
				lua_pushinteger(L, en);
				return nullptr;
			}
			return self;
		}

		static memfile* get(lua_State *L, bool check_close = true) {
			memfile* self = (memfile *)luaL_checkudata(L, 1, MEMFILE_HANDLE);
			if (check_close && self->isclosed()) {
				luaL_error(L, "attempt to use a closed file");
			}
			return self;
		}

		size_t read(void* buf, size_t size) {
			ptrdiff_t n = m_size - m_cur;
			if (n <= 0) {
				return 0;
			}
			n = (std::min)((ptrdiff_t)size, n);
			memcpy(buf, m_data + m_cur, n);
			m_cur += n;
			return n;
		}

		int getc() {
			unsigned char b;
			if (1 == read(&b, 1)) {
				return b;
			}
			return EOF;
		}

		void ungetc(int c) {
			if (c < 0 || m_cur <= 0 || m_data[m_cur - 1] != (unsigned char)(char)c) {
				return;
			}
			--m_cur;
		}

		int seek(ptrdiff_t offset, int mode) {
			int ptr = m_cur;
			switch (mode) {
			case SEEK_SET:
				ptr = offset;
				break;
			case SEEK_CUR:
				ptr += offset;
				break;
			case SEEK_END:
				ptr = m_size + offset;
				break;
			}
			if (ptr < 0) {
				return EINVAL;
			}
			return 0;
		}

		ptrdiff_t tell() const {
			return m_cur;
		}

		bool isclosed() const { return m_data == nullptr; }

		int close(lua_State *L) {
			int en = m_fclose(m_data, m_size);
			m_data = nullptr;
			m_size = 0;
			return pushresult(L, en);
		}

		unsigned char* m_data = nullptr;
		size_t         m_size = 0;
		ptrdiff_t      m_cur = 0;
		IFileClose     m_fclose = nullptr;
	};

	static int f_tostring(lua_State *L) {
		memfile *p = memfile::get(L, false);
		if (p->isclosed())
			lua_pushliteral(L, "file (closed)");
		else
			lua_pushfstring(L, "file (%p)", p);
		return 1;
	}

	static int f_close(lua_State *L) {
		memfile *p = memfile::get(L);
		return p->close(L);
	}

	static int f_gc(lua_State *L) {
		memfile *p = memfile::get(L, false);
		if (p->isclosed()) {
			return 0;
		}
		int r = p->close(L);
		p->~memfile();
		return r;
	}

	static int io_readline(lua_State *L);

	/*
	** maximum number of arguments to 'f:lines'/'io.lines' (it + 3 must fit
	** in the limit for upvalues of a closure)
	*/
#define MAXARGLINE	250

	static int f_lines(lua_State *L) {
		memfile::get(L);  /* check that it's a valid file handle */
		int n = lua_gettop(L) - 1;  /* number of arguments to read */
		luaL_argcheck(L, n <= MAXARGLINE, MAXARGLINE + 2, "too many arguments");
		lua_pushinteger(L, n);  /* number of arguments to read */
		lua_pushboolean(L, 0);  /* close/not close file when finished */
		lua_rotate(L, 2, 2);  /* move 'n' and 'toclose' to their positions */
		lua_pushcclosure(L, io_readline, 3 + n);
		return 1;
	}

	/*
	** {======================================================
	** READ
	** =======================================================
	*/

	/* maximum length of a numeral */
#if !defined (L_MAXLENNUM)
#define L_MAXLENNUM     200
#endif

	/* auxiliary structure used by 'read_number' */
	typedef struct {
		memfile *f;  /* file being read */
		int c;  /* current character (look ahead) */
		int n;  /* number of elements in buffer 'buff' */
		char buff[L_MAXLENNUM + 1];  /* +1 for ending '\0' */
	} RN;

	/*
	** Add current char to buffer (if not out of space) and read next one
	*/
	static int nextc(RN *rn) {
		if (rn->n >= L_MAXLENNUM) {  /* buffer overflow? */
			rn->buff[0] = '\0';  /* invalidate result */
			return 0;  /* fail */
		}
		else {
			rn->buff[rn->n++] = rn->c;  /* save current char */
			rn->c = rn->f->getc();  /* read next one */
			return 1;
		}
	}

	/*
	** Accept current char if it is in 'set' (of size 2)
	*/
	static int test2(RN *rn, const char *set) {
		if (rn->c == set[0] || rn->c == set[1])
			return nextc(rn);
		else return 0;
	}

	/*
	** Read a sequence of (hex)digits
	*/
	static int readdigits(RN *rn, int hex) {
		int count = 0;
		while ((hex ? isxdigit(rn->c) : isdigit(rn->c)) && nextc(rn))
			count++;
		return count;
	}

	/*
	** Read a number: first reads a valid prefix of a numeral into a buffer.
	** Then it calls 'lua_stringtonumber' to check whether the format is
	** correct and to convert it to a Lua number
	*/
	static int read_number(lua_State *L, memfile *f) {
		RN rn;
		int count = 0;
		int hex = 0;
		char decp[2];
		rn.f = f; rn.n = 0;
		decp[0] = lua_getlocaledecpoint();  /* get decimal point from locale */
		decp[1] = '.';  /* always accept a dot */
		do { rn.c = rn.f->getc(); } while (isspace(rn.c));  /* skip spaces */
		test2(&rn, "-+");  /* optional signal */
		if (test2(&rn, "00")) {
			if (test2(&rn, "xX")) hex = 1;  /* numeral is hexadecimal */
			else count = 1;  /* count initial '0' as a valid digit */
		}
		count += readdigits(&rn, hex);  /* integral part */
		if (test2(&rn, decp))  /* decimal point? */
			count += readdigits(&rn, hex);  /* fractional part */
		if (count > 0 && test2(&rn, (hex ? "pP" : "eE"))) {  /* exponent mark? */
			test2(&rn, "-+");  /* exponent signal */
			readdigits(&rn, 0);  /* exponent digits */
		}
		rn.f->ungetc(rn.c);  /* unread look-ahead char */
		rn.buff[rn.n] = '\0';  /* finish string */
		if (lua_stringtonumber(L, rn.buff))  /* is this a valid number? */
			return 1;  /* ok */
		else {  /* invalid format */
			lua_pushnil(L);  /* "result" to be removed */
			return 0;  /* read fails */
		}
	}

	static int test_eof(lua_State *L, memfile *f) {
		int c = f->getc();
		f->ungetc(c);  /* no-op when c == EOF */
		lua_pushliteral(L, "");
		return (c != EOF);
	}

	static int read_line(lua_State *L, memfile *f, int chop) {
		luaL_Buffer b;
		int c = '\0';
		luaL_buffinit(L, &b);
		while (c != EOF && c != '\n') {  /* repeat until end of line */
			char *buff = luaL_prepbuffer(&b);  /* preallocate buffer */
			int i = 0;
			while (i < LUAL_BUFFERSIZE && (c = f->getc()) != EOF && c != '\n')
				buff[i++] = c;
			luaL_addsize(&b, i);
		}
		if (!chop && c == '\n')  /* want a newline and have one? */
			luaL_addchar(&b, c);  /* add ending newline to result */
		luaL_pushresult(&b);  /* close buffer */
							  /* return ok if read something (either a newline or something else) */
		return (c == '\n' || lua_rawlen(L, -1) > 0);
	}

	static void read_all(lua_State *L, memfile *f) {
		size_t nr;
		luaL_Buffer b;
		luaL_buffinit(L, &b);
		do {  /* read file in chunks of LUAL_BUFFERSIZE bytes */
			char *p = luaL_prepbuffer(&b);
			nr = f->read(p, LUAL_BUFFERSIZE);
			luaL_addsize(&b, nr);
		} while (nr == LUAL_BUFFERSIZE);
		luaL_pushresult(&b);  /* close buffer */
	}

	static int read_chars(lua_State *L, memfile *f, size_t n) {
		size_t nr;  /* number of chars actually read */
		char *p;
		luaL_Buffer b;
		luaL_buffinit(L, &b);
		p = luaL_prepbuffsize(&b, n);  /* prepare buffer to read whole block */
		nr = f->read(p, n);  /* try to read 'n' chars */
		luaL_addsize(&b, nr);
		luaL_pushresult(&b);  /* close buffer */
		return (nr > 0);  /* true iff read something */
	}

	static int g_read(lua_State *L, memfile *f, int first) {
		int nargs = lua_gettop(L) - 1;
		int success;
		int n;
		if (nargs == 0) {  /* no arguments? */
			success = read_line(L, f, 1);
			n = first + 1;  /* to return 1 result */
		}
		else {  /* ensure stack space for all results and for auxlib's buffer */
			luaL_checkstack(L, nargs + LUA_MINSTACK, "too many arguments");
			success = 1;
			for (n = first; nargs-- && success; n++) {
				if (lua_type(L, n) == LUA_TNUMBER) {
					size_t l = (size_t)luaL_checkinteger(L, n);
					success = (l == 0) ? test_eof(L, f) : read_chars(L, f, l);
				}
				else {
					const char *p = luaL_checkstring(L, n);
					if (*p == '*') p++;  /* skip optional '*' (for compatibility) */
					switch (*p) {
					case 'n':  /* number */
						success = read_number(L, f);
						break;
					case 'l':  /* line */
						success = read_line(L, f, 1);
						break;
					case 'L':  /* line with end-of-line */
						success = read_line(L, f, 0);
						break;
					case 'a':  /* file */
						read_all(L, f);  /* read entire file */
						success = 1; /* always success */
						break;
					default:
						return luaL_argerror(L, n, "invalid format");
					}
				}
			}
		}
		if (!success) {
			lua_pop(L, 1);  /* remove last result */
			lua_pushnil(L);  /* push nil instead */
		}
		return n - first;
	}

	static int f_read(lua_State *L) {
		return g_read(L, memfile::get(L), 2);
	}

	static int io_readline(lua_State *L) {
		memfile *p = (memfile *)lua_touserdata(L, lua_upvalueindex(1));
		int i;
		int n = (int)lua_tointeger(L, lua_upvalueindex(2));
		if (p->isclosed())  /* file is already closed? */
			return luaL_error(L, "file is already closed");
		lua_settop(L, 1);
		luaL_checkstack(L, n, "too many arguments");
		for (i = 1; i <= n; i++)  /* push arguments to 'g_read' */
			lua_pushvalue(L, lua_upvalueindex(3 + i));
		n = g_read(L, p, 2);  /* 'n' is number of results */
		lua_assert(n > 0);  /* should return at least a nil */
		if (lua_toboolean(L, -n))  /* read at least one value? */
			return n;  /* return them */
		else {  /* first result is nil: EOF or error */
			if (n > 1) {  /* is there error information? */
						  /* 2nd result is error message */
				return luaL_error(L, "%s", lua_tostring(L, -n + 1));
			}
			if (lua_toboolean(L, lua_upvalueindex(3))) {  /* generator created file? */
				lua_settop(L, 0);
				lua_pushvalue(L, lua_upvalueindex(1));
				p->close(L);  /* close it */
			}
			return 0;
		}
	}

	static int f_write(lua_State *L) {
		memfile::get(L);
		return pushresult(L, EBADF);
	}

	static int f_seek(lua_State *L) {
		static const int mode[] = { SEEK_SET, SEEK_CUR, SEEK_END };
		static const char *const modenames[] = { "set", "cur", "end", NULL };
		memfile *f = memfile::get(L);
		int op = luaL_checkoption(L, 2, "cur", modenames);
		lua_Integer p3 = luaL_optinteger(L, 3, 0);
		ptrdiff_t offset = (ptrdiff_t)p3;
		luaL_argcheck(L, (lua_Integer)offset == p3, 3, "not an integer in proper range");

		int err = f->seek(offset, mode[op]);
		if (err) {
			return pushresult(L, err);
		}
		lua_pushinteger(L, (lua_Integer)f->tell());
		return 1;
	}

	static int f_setvbuf(lua_State *L) {
		lua_pushboolean(L, 1);
		return 1;
	}

	static int f_flush(lua_State *L) {
		lua_pushboolean(L, 1);
		return 1;
	}

	static int l_checkmode(const char *mode) {
		return mode[0] == 'r' && (mode[1] == '\0' || (mode[1] == 'b' && mode[2] == '\0'));
	}

	void initialize(lua_State *L)
	{
		static const luaL_Reg lib[] = {
			{ "close", f_close },
			{ "flush", f_flush },
			{ "lines", f_lines },
			{ "read", f_read },
			{ "seek", f_seek },
			{ "setvbuf", f_setvbuf },
			{ "write", f_write },
			{ "__gc", f_gc },
			{ "__tostring", f_tostring },
			{ NULL, NULL }
		};

		lua_pushnil(L);
		lua_setfield(L, LUA_REGISTRYINDEX, MEMFILE_HANDLE);
		luaL_newmetatable(L, MEMFILE_HANDLE);
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, lib, 0);
		lua_pop(L, 1);
	}

	int open(lua_State *L, IFileCreate icreate, IFileClose iclose) {
		const char *filename = luaL_checkstring(L, 1);
		const char *mode = luaL_optstring(L, 2, "r");
		luaL_argcheck(L, l_checkmode(mode), 2, "invalid mode");
		memfile* p = memfile::create(L, filename, icreate, iclose);
		return (p == NULL) ? 3 : 1;
	}
}
