#include "lua_helper.h"
#include "jassbind.h"
#include <base/warcraft3/jass/func_value.h>

namespace base { namespace warcraft3 { namespace lua_engine {
	bool      is_gaming();
	bool      jass_push(lua_State* L, jass::variable_type vt, uint32_t value);
	uintptr_t jass_read(lua_State* L, jass::variable_type opt, int idx);
	int       jass_call_native_function(lua_State* L, const jass::func_value* nf, uintptr_t func_address = 0);
	int       jass_call_closure(lua_State* L);
	lua_State* get_mainthread(lua_State* thread);

}}}
	  
namespace base { namespace warcraft3 { namespace lua_engine { namespace bignum {
	class rsa {
	public:
		rsa();
		~rsa();
		bool check(const char* content, size_t contentlen, const char* sign, size_t signlen);
	private:
		void* encrypt(const char* buf, size_t len);
		const unsigned char* sha1(const char* buf, size_t len);
		void* e;
		void* n;
	};
}}}}
