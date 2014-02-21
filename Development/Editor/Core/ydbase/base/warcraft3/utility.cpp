#include <base/warcraft3/utility.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/warcraft3/version.h>

namespace base { namespace warcraft3 { namespace utility {

	struct mapping_objectid
	{
		uint32_t unk00;
		uint32_t unk01;
		uint32_t unk02;
		uint32_t unk03;
		uint32_t unk04;
		uint32_t unk05;
		uint32_t unk06;
		uint32_t unk07;
		uint32_t unk08;
		uint32_t unk09;
		uint32_t unk0A;
		uint32_t unk0B;
		uint32_t unk0C;
		uint32_t unk0D;
		uint32_t unk0E;
		uint32_t unk0F;
	};

	namespace detail
	{
		mapping_objectid** search_mapping_objectid()
		{
			war3_searcher& s = get_war3_searcher();
			uintptr_t ptr = 0;
			if (s.get_version() > version_121b)
			{
				ptr = s.search_string("e:\\Drive1\\temp\\buildwar3x\\Engine\\Source\\Tempest/tempest_thread.h");
			}
			else
			{
				ptr = s.search_string("..\\Tempest/tempest_thread.h");
			}

			ptr += 0x04;
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr += 0x05;
			ptr = next_opcode(ptr, 0xE8, 5);
			ptr = convert_function(ptr);
			ptr = next_opcode(ptr, 0x89, 6);
			ptr = *(uintptr_t*)(ptr + 0x02);
			return (mapping_objectid**)(ptr);		
		}
	}

	uintptr_t find_objectid_64(const objectid_64& id)
	{
		static mapping_objectid** table_pptr = detail::search_mapping_objectid();

		mapping_objectid* table_ptr = *table_pptr;
		if (!table_ptr)
			return 0;

		if (id.a >> 31)
		{
			if ((id.a & 0x7FFFFFFF) < table_ptr->unk0F)
			{
				if (table_ptr->unk0B && *(uintptr_t*)(table_ptr->unk0B + 8 * id.a) == -2)
				{
					uintptr_t v4 = *(uintptr_t*)(table_ptr->unk0B + 8 * id.a + 4);
					if (v4 && (*(uintptr_t*)(v4 + 0x18) == id.b))
					{
						return v4;
					}
				}
			}
		}
		else
		{
			if (id.a < table_ptr->unk07)
			{
				if (table_ptr->unk03 && *(uintptr_t*)(table_ptr->unk03 + 8 * id.a) == -2)
				{
					uintptr_t v5 = *(uintptr_t*)(table_ptr->unk03 + 8 * id.a + 4);
					if (v5 && (*(uintptr_t*)(v5 + 0x18) == id.b))
					{
						return v5;
					}
				}
			}
		}
		return 0;
	}

	namespace detail
	{
		bool is_valid_ptr(uintptr_t ptr)
		{
			return (get_war3_searcher().base() & 0xFF000000) == (ptr & 0xFF000000);
		}
	}

	const char* get_class_name(uintptr_t ptr)
	{
		if (ptr && !::IsBadReadPtr(reinterpret_cast<const void*>(ptr), sizeof(uintptr_t)))
		{
			ptr = *(uintptr_t*)ptr - 0x04;
			if (detail::is_valid_ptr(ptr))
			{
				ptr = *(uintptr_t*)ptr + 0x0C;
				if (detail::is_valid_ptr(ptr))
				{
					ptr = *(uintptr_t*)ptr + 0x08;
					return (const char*)ptr;
				}
			}
		}
		return nullptr;
	}
}}}
