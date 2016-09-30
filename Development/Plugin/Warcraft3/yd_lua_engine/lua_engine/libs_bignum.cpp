#include "lua_helper.h"	  
#include "storm.h"
#include <base/hook/fp_call.h>
#include <assert.h>

namespace base { namespace warcraft3 { namespace lua_engine { namespace bignum {

#include "sha_1.h"	 
#include "sha_1.inl"

	namespace sbig {
		static void Del(HANDLE big)
		{
			static uintptr_t func = storm_s::instance().get_proc(606);
			std_call<void>(func, big);
		}

		static void FromBinary(HANDLE big, const char* buf, size_t len)
		{
			static uintptr_t func = storm_s::instance().get_proc(609);
			std_call<void>(func, big, buf, len);
		}

		static void New(HANDLE* big)
		{
			static uintptr_t func = storm_s::instance().get_proc(624);
			std_call<void>(func, big);
		}

		static void PowMod(HANDLE dst, HANDLE src, HANDLE pow, HANDLE mod)
		{
			static uintptr_t func = storm_s::instance().get_proc(628);
			std_call<void>(func, dst, src, pow, mod);
		}

		static void ToBinaryBuffer(HANDLE big, char* buf, size_t len, size_t* wlen)
		{
			static uintptr_t func = storm_s::instance().get_proc(638);
			std_call<void>(func, big, buf, len, wlen);
		}
	}

	namespace lbig {
		static HANDLE create(lua_State* L)
		{
			HANDLE* big = (HANDLE*)lua_newuserdata(L, sizeof(HANDLE));
			luaL_getmetatable(L, "jbignum_t");
			lua_setmetatable(L, -2);
			sbig::New(big);
			return *big;
		}

		static int new_(lua_State* L)
		{
			HANDLE big = create(L);
			size_t len = 0;
			const char* buf = lua_tolstring(L, 1, &len);
			sbig::FromBinary(big, buf, len);
			return 1;
		}

		static int gc(lua_State* L)
		{
			HANDLE* big = (HANDLE*)lua_touserdata(L, 1);
			sbig::Del(*big);
			*big = 0;
			return 0;
		}

		static int tostring(lua_State* L)
		{
			HANDLE big = *(HANDLE*)lua_touserdata(L, 1);
			size_t len = 0x100;
			size_t wlen = 0;
			char* buf = 0;
			for (;;) {
				buf = (char*)lua_newuserdata(L, len + 1);
				wlen = 0;
				sbig::ToBinaryBuffer(big, buf, len + 1, &wlen);
				if (wlen < len + 1) {
					break;
				}
				lua_pop(L, 1);
				len *= 2;
			} 
			lua_pushlstring(L, buf, wlen);
			return 1;
		}

		static int len(lua_State* L)
		{
			HANDLE big = *(HANDLE*)lua_touserdata(L, 1);
			size_t len = 0x100;
			size_t wlen = 0;
			char* buf = 0;
			for (;;) {
				buf = (char*)lua_newuserdata(L, len + 1);
				wlen = 0;
				sbig::ToBinaryBuffer(big, buf, len + 1, &wlen);
				if (wlen < len + 1) {
					break;
				}
				lua_pop(L, 1);
				len *= 2;
			}
			lua_pushinteger(L, wlen);
			return 1;
		}

		static int powmod(lua_State* L)
		{
			HANDLE src = *(HANDLE*)lua_touserdata(L, 1);
			HANDLE pow = *(HANDLE*)lua_touserdata(L, 2);
			HANDLE mod = *(HANDLE*)lua_touserdata(L, 3);
			HANDLE dst = create(L);
			sbig::PowMod(dst, src, pow, mod);
			return 1;
		}
	}	   

	static int lbin(lua_State* L)
	{
		static unsigned char binstr[] = {
			/*         0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F */
			/* 0x00 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0x10 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0x20 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0x30 */ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0x40 */ 0x00, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0x50 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0x60 */ 0x00, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0x70 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0x80 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0x90 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0xA0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0xB0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0xC0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0xD0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0xE0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
			/* 0xF0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		};
		size_t hexlen = 0;
		const char* hex = lua_tolstring(L, 1, &hexlen);
		assert(hexlen % 2 == 0);
		size_t binlen = hexlen / 2;
		char* bin = (char*)lua_newuserdata(L, binlen);
		for (size_t i = 0; i < binlen; ++i)
		{
			bin[binlen - i - 1] = (binstr[hex[2 * i + 0]] << 4) + binstr[hex[2 * i + 1]];
		} 	
		lua_pushlstring(L, bin, binlen);
		return 1;
	}

	static int lhex(lua_State* L)
	{
		static char hexstr[] = "0123456789ABCDEF";
		size_t binlen = 0;
		const char* bin = lua_tolstring(L, 1, &binlen);
		size_t hexlen = binlen * 2;
		char* hex = (char*)lua_newuserdata(L, hexlen);
		for (size_t i = 0; i < binlen; ++i)
		{
			hex[hexlen - 2 * i - 2] = hexstr[(bin[i] >> 4) & 0xF];
			hex[hexlen - 2 * i - 1] = hexstr[bin[i] & 0xF];
		}
		lua_pushlstring(L, hex, hexlen);
		return 1;
	}

	static int lsha1(lua_State* L)
	{
		size_t len = 0;
		const unsigned char* buf = (const unsigned char*)lua_tolstring(L, 1, &len);
		unsigned char sha1out[SHA1HashSize] = { 0 };
		SHA1Context context;
		SHA1Reset(&context);
		SHA1Input(&context, buf, len);
		SHA1Result(&context, sha1out);
		lua_pushlstring(L, (const char*)sha1out, SHA1HashSize);
		return 1;
	}
	
	int open(lua_State* L)
	{
		luaL_Reg meta[] = {
			{ "__len", lbig::len },
			{ "__tostring", lbig::tostring },
			{ "__gc", lbig::gc },
			{ "powmod", lbig::powmod },
			{ NULL, NULL },
		};
		luaL_newmetatable(L, "jbignum_t");
		lua_pushvalue(L, -1);
		lua_setfield(L, -2, "__index");
		luaL_setfuncs(L, meta, 0);
		lua_pop(L, 1);

		lua_newtable(L);
		luaL_Reg lib[] = {
			{ "new", lbig::new_ },
			{ "bin", lbin },
			{ "hex", lhex },
			{ "sha1", lsha1 },
			{ NULL, NULL },
		};
		luaL_setfuncs(L, lib, 0);
		return 1;
	}
}}}}
