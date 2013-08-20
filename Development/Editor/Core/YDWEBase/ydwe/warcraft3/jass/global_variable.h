#pragma once

#include <ydwe/config.h>
#include <ydwe/warcraft3/jass.h>
#include <ydwe/warcraft3/jass/opcode.h>
#include <ydwe/warcraft3/hashtable.h>
#include <ydwe/warcraft3/native_function.h>

_BASE_BEGIN 
namespace warcraft3 { namespace jass {

	inline native_function::variable_type opcode_type_to_var_type(jass::OPCODE_VARIABLE_TYPE opt)
	{
		switch (opt)
		{
		case jass::OPCODE_VARIABLE_NOTHING:
		case jass::OPCODE_VARIABLE_UNKNOWN:
		case jass::OPCODE_VARIABLE_NULL:
			return native_function::TYPE_NOTHING;
		case jass::OPCODE_VARIABLE_CODE:
			return native_function::TYPE_CODE;
		case jass::OPCODE_VARIABLE_INTEGER:
			return native_function::TYPE_INTEGER;
		case jass::OPCODE_VARIABLE_REAL:
			return native_function::TYPE_REAL;
		case jass::OPCODE_VARIABLE_STRING:
			return native_function::TYPE_STRING;
		case jass::OPCODE_VARIABLE_HANDLE:
			return native_function::TYPE_HANDLE;
		case jass::OPCODE_VARIABLE_BOOLEAN:
			return native_function::TYPE_BOOLEAN;
		case jass::OPCODE_VARIABLE_INTEGER_ARRAY:
		case jass::OPCODE_VARIABLE_REAL_ARRAY:
		case jass::OPCODE_VARIABLE_STRING_ARRAY:
		case jass::OPCODE_VARIABLE_HANDLE_ARRAY:
		case jass::OPCODE_VARIABLE_BOOLEAN_ARRAY:
			return native_function::TYPE_NOTHING;
		default:
			return native_function::TYPE_NOTHING;
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
_BASE_END 
