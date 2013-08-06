#pragma once

#include <ydwe/config.h>
#include <ydwe/warcraft3/jass.h>
#include <ydwe/warcraft3/jass/opcode.h>
#include <ydwe/warcraft3/hashtable.h>

_BASE_BEGIN 
namespace warcraft3 { namespace jass {

	class _BASE_API global_variable
	{
	public:
		global_variable(const char* name);
		OPCODE_VARIABLE_TYPE type() const;
		bool                 is_array() const;
		operator bool              () const;
		operator uint32_t&         ();
		operator uint32_t const&   () const;
		global_variable& operator =(uint32_t v);

	private:
		hashtable::variable_node* ptr_;
	};
}}
_BASE_END 
