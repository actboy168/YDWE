#pragma once

#include <base/config.h>
#include <base/warcraft3/jass.h>
#include <base/warcraft3/jass/opcode.h>
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/jass/func_value.h>

namespace base { namespace warcraft3 { namespace jass {

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
			return TYPE_INTEGER;
		case OPCODE_VARIABLE_REAL_ARRAY:
			return TYPE_REAL;
		case OPCODE_VARIABLE_STRING_ARRAY:
			return TYPE_STRING;
		case OPCODE_VARIABLE_HANDLE_ARRAY:
			return TYPE_HANDLE;
		case OPCODE_VARIABLE_BOOLEAN_ARRAY:
			return TYPE_BOOLEAN;
		default:
			return TYPE_NOTHING;
		}
	}

	inline OPCODE_VARIABLE_TYPE opcode_type_remove_array(OPCODE_VARIABLE_TYPE opt)
	{
		switch (opt)
		{
		case OPCODE_VARIABLE_INTEGER_ARRAY:
			return OPCODE_VARIABLE_INTEGER;
		case OPCODE_VARIABLE_REAL_ARRAY:
			return OPCODE_VARIABLE_REAL;
		case OPCODE_VARIABLE_STRING_ARRAY:
			return OPCODE_VARIABLE_STRING;
		case OPCODE_VARIABLE_HANDLE_ARRAY:
			return OPCODE_VARIABLE_HANDLE;
		case OPCODE_VARIABLE_BOOLEAN_ARRAY:
			return OPCODE_VARIABLE_BOOLEAN;
		default:
			return opt;
		}
	}


	class _BASE_API global_variable
	{
	public:
		struct array_entry;

		global_variable(const char* name);
		global_variable(hashtable::variable_node* ptr);
		hashtable::variable_node*  ptr() const;
		OPCODE_VARIABLE_TYPE       type() const;
		const char*                name() const;
		bool                       is_array() const;
		bool                       is_vaild() const;
		operator uint32_t&         ();
		operator uint32_t const&   () const;
		operator array_entry*      () const;
		global_variable& operator =(uint32_t v);

		array_entry*               entry() const;
		uint32_t                   array_size() const;
		bool                       array_vaild(int32_t index) const;
		uint32_t&                  operator [](int32_t index);
		const uint32_t&            operator [](int32_t index) const;

	private:
		hashtable::variable_node* ptr_;
	};
}}} 
