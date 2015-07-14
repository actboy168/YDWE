#pragma once

#include <array>
#include <map>
#include <queue>
#include <base/util/foreach.h>

namespace base { namespace warcraft3 { namespace japi {

	class string_pool_t
	{
	public:
		~string_pool_t()
		{
			foreach(auto it, used_)
			{
				::free((void*)(it.first));
			}

			for (; !fl_080_.empty();)
			{
				::free((void*)fl_080_.front());
				fl_080_.pop();
			}

			for (; !fl_100_.empty();)
			{
				::free((void*)fl_100_.front());
				fl_100_.pop();
			}

			for (; !fl_200_.empty();)
			{
				::free((void*)fl_200_.front());
				fl_200_.pop();
			}

			for (; !fl_400_.empty();)
			{
				::free((void*)fl_400_.front());
				fl_400_.pop();
			}
		}

		bool is_used(uintptr_t ptr) const
		{
			return used_.find(ptr) != used_.end();
		}

		bool free(uintptr_t ptr)
		{
			auto it = used_.find(ptr);
			if (it == used_.end())
			{
				return false;
			}

			switch (it->second)
			{
			case 0x080: fl_080_.push(ptr); break;
			case 0x100: fl_100_.push(ptr); break;
			case 0x200: fl_200_.push(ptr); break;
			case 0x400: fl_400_.push(ptr); break;
			default: ::free((void*)(ptr)); break;
			}

			used_.erase(it);
			return true;
		}

		uintptr_t malloc(size_t size)
		{
			uintptr_t ret = 0;
			if (size <= 0x080)
			{
				if (fl_080_.empty())
				{
					ret = (uintptr_t)::malloc(0x080);
				}
				else
				{
					ret = fl_080_.front();
					fl_080_.pop();
				}

				if (ret != 0)
				{
					used_[ret] = 0x080;
				}
			}
			else if (size <= 0x100)
			{
				if (fl_100_.empty())
				{
					ret = (uintptr_t)::malloc(0x100);
				}
				else
				{
					ret = fl_100_.front();
					fl_100_.pop();
				}

				if (ret != 0)
				{
					used_[ret] = 0x100;
				}
			}
			else if (size <= 0x200)
			{
				if (fl_200_.empty())
				{
					ret = (uintptr_t)::malloc(0x200);
				}
				else
				{
					ret = fl_200_.front();
					fl_200_.pop();
				}

				if (ret != 0)
				{
					used_[ret] = 0x200;
				}
			}
			else if (size <= 0x400)
			{
				if (fl_400_.empty())
				{
					ret = (uintptr_t)::malloc(0x400);
				}
				else
				{
					ret = fl_400_.front();
					fl_400_.pop();
				}

				if (ret != 0)
				{
					used_[ret] = 0x400;
				}
			}
			else
			{
				ret = (uintptr_t)::malloc(size);
				if (ret != 0)
				{
					used_[ret] = size;
				}
			}

			return ret;
		}

	private:
		std::map<uintptr_t, size_t> used_;

		std::queue<uintptr_t> fl_080_;
		std::queue<uintptr_t> fl_100_;
		std::queue<uintptr_t> fl_200_;
		std::queue<uintptr_t> fl_400_;
	};
}}}
