#pragma once

#include <base/config.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/opcode.h>
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/jass/func_value.h>

namespace base { 
namespace warcraft3 { namespace jass {

	inline variable_type opcode_type_to_var_type(OPCODE_VARIABLE_TYPE opt)
	{
		switch (opt)
		{
		case OPCODE_VARIABLE_NOTHING:
		case OPCODE_VARIABLE_UNKNOWN:
		case OPCODE_VARIABLE_NULL:
			return TYPE_NOTHING;
		case OPCODE_VARIABLE_CODE:
			return TYPE_CODE;
		case OPCODE_VARIABLE_INTEGER:
			return TYPE_INTEGER;
		case OPCODE_VARIABLE_REAL:
			return TYPE_REAL;
		case OPCODE_VARIABLE_STRING:
			return TYPE_STRING;
		case OPCODE_VARIABLE_HANDLE:
			return TYPE_HANDLE;
		case OPCODE_VARIABLE_BOOLEAN:
			return TYPE_BOOLEAN;
		case OPCODE_VARIABLE_INTEGER_ARRAY:
		case OPCODE_VARIABLE_REAL_ARRAY:
		case OPCODE_VARIABLE_STRING_ARRAY:
		case OPCODE_VARIABLE_HANDLE_ARRAY:
		case OPCODE_VARIABLE_BOOLEAN_ARRAY:
			return TYPE_NOTHING;
		default:
			return TYPE_NOTHING;
		}
	}

	class _BASE_API global_variable
	{
	public:
		global_variable(const char* name);
		OPCODE_VARIABLE_TYPE type() const;
		bool                 is_array() const;
		bool                 is_vaild() const;
		operator uint32_t&         ();
		operator uint32_t const&   () const;
		global_variable& operator =(uint32_t v);

	private:
		hashtable::variable_node* ptr_;
	};
}}
} 
