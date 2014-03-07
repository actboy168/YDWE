#include "../lua/helper.h"
#include "../lua/jassbind.h"
#include <base/warcraft3/jass/func_value.h>

namespace base { namespace warcraft3 { namespace lua_engine {
	bool      is_gaming();
	bool      jass_push(jassbind* lj, jass::variable_type vt, uint32_t value);
	uintptr_t jass_read(jassbind* lj, jass::variable_type opt, int idx);
	int       jass_call_native_function(jassbind* lj, const jass::func_value* nf, uintptr_t func_address = 0);
	int       jass_call_closure(lua_State* L);
}}}
