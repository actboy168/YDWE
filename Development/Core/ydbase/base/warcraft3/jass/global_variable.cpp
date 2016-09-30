#include <base/warcraft3/jass/global_variable.h>
#include <cassert>

namespace base { namespace warcraft3 { namespace jass {

	global_variable::global_variable(const char* name)
		: ptr_(get_jass_vm()->global_table->get(name))
	{ }

	global_variable::global_variable(hashtable::variable_node* ptr)
		: ptr_(ptr)
	{ }

	hashtable::variable_node*  global_variable::ptr() const
	{
		return ptr_;
	}

	OPCODE_VARIABLE_TYPE global_variable::type() const
	{
		assert(is_vaild());
		return static_cast<OPCODE_VARIABLE_TYPE>(ptr_->type_);
	}

	const char* global_variable::name() const
	{
		return ptr_->str_;
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

	struct global_variable::array_entry
	{
		uint32_t  unk0;
		uint32_t  unk2;
		uint32_t  size;
		uint32_t* data;
	};

	global_variable::array_entry* global_variable::entry() const
	{
		assert(is_vaild());
		assert(is_array());
		return (array_entry*)ptr_->value_; 
	}

	uint32_t global_variable::array_size() const
	{
		return entry()? entry()->size: 0;
	}

	bool global_variable::array_vaild(int32_t index) const
	{
		return entry()? ((index >= 0) && (array_size() > (uint32_t)index)): false;
	}

	uint32_t& global_variable::operator [](int32_t index)
	{
		assert(is_array());
		assert(array_vaild(index));
		return entry()->data[index];
	}

	const uint32_t& global_variable::operator [](int32_t index) const
	{
		assert(is_array());
		assert(array_vaild(index));
		return entry()->data[index];
	}
}}} 
