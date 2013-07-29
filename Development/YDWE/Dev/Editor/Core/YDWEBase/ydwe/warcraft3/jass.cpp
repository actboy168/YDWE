#include <ydwe/warcraft3/jass.h>
#include <ydwe/warcraft3/war3_searcher.h>
#include <ydwe/warcraft3/detail/memory_search.h>
#include <ydwe/warcraft3/native_function.h>
#include <memory>

namespace ydwe { namespace warcraft3 { namespace jass {

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

	string_fake::~string_fake()
	{
		memory_[7] = 0;
	}

	string_fake::operator jstring_t () const
	{
		return (jstring_t)memory_;
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
		ptr = detail::next_opcode(ptr, 0xE8, 5);
		ptr += 0x05;
		ptr = detail::next_opcode(ptr, 0xE8, 5);
		return detail::convert_function(ptr);
	}

	uintptr_t create_string(const char* val)
	{
		static uintptr_t s_create_string = search_create_string();
		return ((uintptr_t(_fastcall*)(const char*))s_create_string)(val);
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
		if (0 == strncmp(val, "TRIGSTR_", _countof("TRIGSTR_")-1))
		{
			uint32_t index = detail::str_to_uint(&(val[_countof("TRIGSTR_")-1]));
			return detail::trigstring_table::get()->find(index, val);
		}

		return val;
	}

	uintptr_t  call(const char* name, ...)
	{
		native_function::native_function const* nf = native_function::jass_func(name);
		if (!nf) return 0;

		uintptr_t func_address = nf->get_address();
		size_t    param_size   = nf->get_param().size() * sizeof uintptr_t;
		uintptr_t retval;
		uintptr_t esp_ptr;

		va_list ap;
		va_start(ap, name);

		_asm
		{
			sub  esp, param_size
			mov  esp_ptr, esp
		}
		memcpy((void*)esp_ptr, ap, param_size);
		_asm
		{
			call [func_address]
			mov  esp, esp_ptr
			add  esp, param_size
			mov  retval, eax
		}

		va_end(ap);

		return retval;
	}
}}}
