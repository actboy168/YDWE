#include <base/warcraft3/message_dispatch.h>
#include <base/warcraft3/war3_searcher.h>	
#include <base/warcraft3/version.h>		 
#include <base/hook/inline.h>	  		 
#include <base/hook/fp_call.h>
#include <vector>

namespace base { namespace warcraft3 { namespace message_dispatch {

	static uintptr_t fn_real = 0;
	std::vector<uintptr_t> filters;

	static uintptr_t search()
	{
		war3_searcher& s = get_war3_searcher();
		if (s.get_version() == version_120e)
		{
			return s.base() + 0x0070CD34;
		}
		else if (s.get_version() < version_127a)
		{
			uintptr_t ptr = s.search_string_ptr("QuickSave", sizeof("QuickSave"));
			ptr += 0x44;
			return ptr;
		}
		uintptr_t ptr = s.search_string_ptr("d:\\buildserver\\3\\work-git\\warcraft3-repository\\war3\\source\\ui\\CMultiboard.h", sizeof("d:\\buildserver\\3\\work-git\\warcraft3-repository\\war3\\source\\ui\\CMultiboard.h"));
		ptr += 0x80;
		return ptr;
	}

	static uintptr_t get()
	{
		static uintptr_t ptr = search();
		return ptr;
	}

	static bool __fastcall fn_fake(uintptr_t cgameui, uintptr_t /*edx*/, void* msg)
	{
		for (auto it = filters.begin(); it != filters.end(); ++it)
		{
			if (!this_call<bool>(*it, cgameui, msg))
			{
				return false;
			}
		}
		return this_call<bool>(fn_real, cgameui, msg);
	}

	void add_filter(uintptr_t filter)
	{
		filters.clear();
		filters.push_back(filter);
		if (!fn_real)
		{
			fn_real = hook::replace_pointer(get(), (uintptr_t)fn_fake);
		}
	}

	void remove_filter(uintptr_t filter)
	{
		for (auto it = filters.begin(); it != filters.end(); ++it)
		{
			if (*it == filter)
			{
				filters.erase(it);
				break;
			}
		}
	}
}}}
