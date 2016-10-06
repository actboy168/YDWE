#include <base/warcraft3/jass.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/hashtable.h>
#include <base/warcraft3/jass/func_value.h>
#include <base/hook/fp_call.h>
#include <cassert>
#include <memory>

namespace base { namespace warcraft3 { namespace jass {

	string_fake::string_fake() 
	{
		memory_[2] = (uint32_t)&memory_[0];
		memory_[7] = 0;
	}

	string_fake::string_fake(const char* str) 
	{
		memory_[2] = (uint32_t)&memory_[0];
		memory_[7] = (uint32_t)&str[0];
	}

	string_fake::string_fake(const string_fake& that)
	{
		memory_[2] = (uint32_t)&memory_[0];
		memory_[7] = that.memory_[7];
	}

	string_fake::string_fake(string_fake&& that)
	{
		memory_[2] = (uint32_t)&memory_[0];
		memory_[7] = that.memory_[7];
	}

	string_fake::~string_fake()
	{
		memory_[7] = 0;
	}

	string_fake::operator jstring_t () const
	{
		return (jstring_t)memory_;
	}

	string_fake& string_fake::operator =(const char* str)
	{
		memory_[7] = (uint32_t)&str[0];
		return *this;
	}

	string_fake& string_fake::operator =(const string_fake& that)
	{
		if (this != &that)
		{
			memory_[7] = that.memory_[7];
		}
		return *this;
	}

	string_fake& string_fake::operator =(string_fake&& that)
	{
		if (this != &that)
		{
			memory_[7] = that.memory_[7];
		}
		return *this;
	}

	const char* from_string(jstring_t val)
	{
		if (!val)
		{
			return nullptr;
		}

		val = *(uint32_t *)((uint32_t)val + 8);

		if (!val)
		{
			return nullptr;
		}

		return (const char*)(*(uint32_t *)(val + 28));
	}

	string_fake to_string(const char* val)
	{
		return std::move(string_fake(val));
	}

	uintptr_t search_create_string()
	{
		uintptr_t ptr = get_war3_searcher().search_string("I2S");
		ptr = *(uintptr_t*)(ptr + 0x05);
		ptr = next_opcode(ptr, 0xE8, 5);
		ptr += 0x05;
		ptr = next_opcode(ptr, 0xE8, 5);
		return convert_function(ptr);
	}

	jstring_t create_string(const char* val)
	{
		static uintptr_t s_create_string = search_create_string();
		return ((jstring_t(_fastcall*)(const char*))s_create_string)(val);
	}

	namespace detail
	{
		unsigned int str_to_uint(const char* str)
		{
			unsigned int sum = 0;
			for(const char* c = str; *c; ++c)
			{
				if (!isdigit(static_cast<uint8_t>(*c)))
				{
					break;
				}

				sum = sum * 10 + (*c-'0');
			}

			return sum;
		}

		struct trigstring
		{
			uint32_t     index_;
			uint32_t     unk04_;
			uint32_t     unk08_;
			uint32_t     unk0C_;
			const char*  str_;
			uint8_t      unk[304];
		};

		struct trigstring_table
		{
			uint32_t     unk00_;
			uint32_t     count_;
			trigstring*  table_;

			static trigstring_table* get()
			{
				uintptr_t ptr = get_war3_searcher().get_instance(13);
				if (ptr)
				{
					ptr = *(uintptr_t*)(*(uintptr_t*)(ptr + 16) + 24);
					if (ptr)
					{
						return (trigstring_table*)*(uintptr_t*)(ptr + 1004);
					}
				}

				return nullptr;
			}

			const char* find(uint32_t index, const char* def)
			{
				if (this) 
				{
					for (trigstring* it = this->table_; it < &(this->table_[this->count_]); ++it)
					{
						if (it->index_ == index)
						{
							return it->str_;
						}
					}
				}

				return def;
			}
		};
	}

	const char* from_trigstring(const char* val)
	{
		if (val && (0 == strncmp(val, "TRIGSTR_", _countof("TRIGSTR_")-1)))
		{
			uint32_t index = detail::str_to_uint(&(val[_countof("TRIGSTR_")-1]));
			return detail::trigstring_table::get()->find(index, val);
		}

		return val;
	}

	const char* from_stringid(uint32_t strid)
	{
		return get_jass_vm()->symbol_table->string_table->at(strid)->str_;
	}

	void handle_set_ref(jass::jhandle_t h, bool dec)
	{
		jass_vm_t* vm = get_jass_vm();
		if (vm && vm->set_handle_reference)
		{
			fast_call<void>(vm->set_handle_reference, h, dec ? 1 : 0, vm->handle_table);
		}
	}

	void handle_ref(jass::jhandle_t h)
	{
		handle_set_ref(h, false);
	}

	void handle_unref(jass::jhandle_t h)
	{
		handle_set_ref(h, true);
	}

	call_param::call_param(size_t n)
		: param_buffer_(n)
		, real_buffer_(n)
		, string_buffer_(n)
	{ }

	template <> _BASE_API
	void call_param::push<uintptr_t>(size_t i, uintptr_t value)
	{
		assert(i < param_buffer_.size());
		param_buffer_[i] = value;
	}

	template <> _BASE_API
	void call_param::push<intptr_t>(size_t i, intptr_t value)
	{
		assert(i < param_buffer_.size());
		param_buffer_[i] = *(uintptr_t*)&value;
	}

	template <> _BASE_API
	void call_param::push<bool>(size_t i, bool value)
	{
		assert(i < param_buffer_.size());
		param_buffer_[i] = value? 1: 0;
	}

	template <> _BASE_API
	void call_param::push<float>(size_t i, float value)
	{
		push_real(i, to_real(value));
	}

	void call_param::push_real(size_t i, jreal_t value)
	{
		assert(i < param_buffer_.size());
		real_buffer_[i] = value;
		param_buffer_[i] = (uintptr_t)&real_buffer_[i];
	}

	template <> _BASE_API
	void call_param::push<const char*>(size_t i, const char* value)
	{
		assert(i < param_buffer_.size());
		string_buffer_[i] = to_string(value);
		param_buffer_[i] = (jstring_t)string_buffer_[i];
	}

	const uintptr_t* call_param::data() const 
	{
		return param_buffer_.data();
	}

	size_t           call_param::size() const
	{
		return param_buffer_.size();
	}

	uintptr_t  call(const char* name, ...)
	{
		func_value const* nf = japi_func(name);
		if (!nf) {
			nf = jass_func(name);
			if (!nf) {
				return 0;
			}
		}
		return nf->call((const uintptr_t*)((va_list)_ADDRESSOF(name) + _INTSIZEOF(name)));
	}

	uintptr_t call(uintptr_t func_address, const uintptr_t* param_list, size_t param_list_size)
	{
		uintptr_t retval;
		uintptr_t esp_ptr;
		size_t  param_size = param_list_size * sizeof uintptr_t;

		_asm
		{
			sub  esp, param_size;
			mov  esp_ptr, esp;
		}
		memcpy((void*)esp_ptr, param_list, param_size);
		_asm
		{
			call [func_address];
			mov  esp, esp_ptr;
			add  esp, param_size;
			mov  retval, eax;
		}

		return retval;
	}
}}}
