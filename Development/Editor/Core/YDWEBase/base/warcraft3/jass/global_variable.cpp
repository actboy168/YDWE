#include <base/warcraft3/jass/global_variable.h>

_BASE_BEGIN 
namespace warcraft3 { namespace jass {

	global_variable::global_variable(const char* name)
		: ptr_(get_variable_hashtable()->get(name))
	{ }
	
	OPCODE_VARIABLE_TYPE global_variable::type() const
	{
		assert(is_vaild());
		return static_cast<OPCODE_VARIABLE_TYPE>(ptr_->type_);
	}
	
	bool global_variable::is_array() const
	{
		switch (type())
		{
		case OPCODE_VARIABLE_INTEGER_ARRAY:
		case OPCODE_VARIABLE_REAL_ARRAY:
		case OPCODE_VARIABLE_STRING_ARRAY:
		case OPCODE_VARIABLE_HANDLE_ARRAY:
		case OPCODE_VARIABLE_BOOLEAN_ARRAY:
			return true;
		default:
			break;
		}
	
		return false;
	}

	bool global_variable::is_vaild() const
	{
		return !!ptr_;
	}

	global_variable::operator uint32_t& ()
	{
		assert(is_vaild());
		assert(!is_array());
		return ptr_->value_;
	}

	global_variable::operator uint32_t const&   () const
	{
		assert(is_vaild());
		assert(!is_array());
		return ptr_->value_; 
	}

	global_variable& global_variable::operator =(uint32_t v)
	{
		assert(is_vaild());
		assert(!is_array());
		ptr_->value_ = v;
		return *this;

	}
}}

_BASE_END 
