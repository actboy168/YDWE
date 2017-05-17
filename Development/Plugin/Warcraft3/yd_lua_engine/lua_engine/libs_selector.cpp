#include <lua.hpp>
#include <base/warcraft3/war3_searcher.h>
#include <assert.h>
#include "jassbind.h"

namespace base { namespace warcraft3 { namespace lua_engine { namespace selector {

	template <typename T>
	class array_view {
	public:
		typedef const T* const_iterator;
		typedef const_iterator iterator;

		array_view()
			: m_data(nullptr)
			, m_size(0)
		{ }
		array_view(T* data, size_t size)
			: m_data(data)
			, m_size(size)
		{ }
		size_t size() const {
			return m_size;
		}
		bool empty() const {
			return !m_size || !m_data; 
		}
		const_iterator begin() const {
			return m_data;
		}
		const_iterator end() const {
			return begin() + size();
		}

	private:
		T*     m_data;
		size_t m_size;
	};

	typedef int32_t unit_t;
	typedef array_view<unit_t> unitarray_t;

	static unitarray_t GetUnitArray()
	{
		uintptr_t CGameUI = get_war3_searcher().get_gameui(0, 0);
		if (CGameUI)
		{
			uint32_t UnitsTable = *(uint32_t*)(CGameUI + 0x3BC);
			if (UnitsTable)
			{
				return unitarray_t(*(unit_t**)(UnitsTable + 0x608), *(size_t *)(UnitsTable + 0x604));
			}
		}
		return unitarray_t();
	}

	bool IsVaildUnit(unit_t u)
	{
		static uintptr_t uvt = get_vfn_ptr(".?AVCUnit@@");
		if (u <= 0) return false;
		if (*((uint8_t*)&uvt + 0) != *(uint8_t*)(u + 0))
			return false;
		else if (*((uint8_t*)&uvt + 1) != *(uint8_t*)(u + 1))
			return false;
		else if (*((uint8_t*)&uvt + 2) != *(uint8_t*)(u + 2))
			return false;
		else if (*((uint8_t*)&uvt + 3) != *(uint8_t*)(u + 3))
			return false;
		uint32_t x1 = *(uint32_t*)(u + 0xC);
		uint32_t y1 = *(uint32_t*)(u + 0x10);
		uint32_t udata = *(uint32_t*)(u + 0x28);
		if (x1 == 0xFFFFFFFF || y1 == 0xFFFFFFFF || udata == 0)
			return false;
		uint32_t flag = *(uint32_t*)(u + 0x20);
		uint32_t flag2 = *(uint32_t*)(u + 0x5C);
		if (flag & 1u)
			return false;
		if (!(flag & 2u))
			return false;
		if (flag2 & 0x100u)
			return false;
		if (flag2 == 0x1001u)
			return false;
		return true;
	}

	bool IsHero(unit_t u)
	{
		assert(IsVaildUnit(u));
		uint32_t unitid = *(uint32_t*)(u + 0x30);
		return isupper(unitid >> 24);
	}

	static int hero(lua_State* L)
	{
		unitarray_t unitarray = GetUnitArray();
		lua_createtable(L, unitarray.size(), 0);
		if (unitarray.empty()) {
			return 1;
		}
		lua_Integer n = 0;
		for (unit_t u : unitarray)
		{
			if (IsVaildUnit(u) && IsHero(u))
			{
				jassbind::push_handle(L, object_to_handle(u));
				lua_rawseti(L, -2, ++n);
			}
		}
		return 1;
	}

	int open(lua_State* L)
	{
		luaL_Reg f[] = {
			{ "hero", hero },
			{ NULL, NULL },
		};
		luaL_newlib(L, f);
		return 1;
	}
}}}}
