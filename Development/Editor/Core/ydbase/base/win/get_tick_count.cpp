#include <base/win/get_tick_count.h>
#include <base/thread/lock/spin.h>
#include <base/thread/lock/guard.h>
#include <base/util/singleton.h>
#include <windows.h>

namespace base { namespace win {

	namespace detail
	{
		class get_tick_count
		{
		public:
			get_tick_count()
				: ptr_(reinterpret_cast<GetTickCount64_t>(::GetProcAddress(::GetModuleHandleW(L"kernel32.dll"), "GetTickCount64")))
			{
				if (!ptr_)
				{
					ptr_ = custom;
				}
			}

			uint64_t call() const
			{
				return ptr_();
			}

		private:
			typedef ULONGLONG(WINAPI* GetTickCount64_t)();
			GetTickCount64_t ptr_;

		private:
			static uint32_t     rollover_time;
			static uint32_t     lastnow_time;
			static lock::spin<> gtc_lock;

			static uint64_t __stdcall custom()
			{
				uint32_t now = ::GetTickCount();

				lock::guard<lock::spin<>>  lock(gtc_lock);

				if ((now < lastnow_time) && ((lastnow_time - now) >(1 << 30)))
				{
					++rollover_time;
				}
				lastnow_time = now;

				return (((uint64_t)rollover_time) << 32) | now;
			}
		};

		uint32_t  get_tick_count::rollover_time = 0;
		uint32_t  get_tick_count::lastnow_time = 0;
		lock::spin<> get_tick_count::gtc_lock;
	}

	uint64_t get_tick_count()
	{
		return singleton_threadsafe<detail::get_tick_count>::instance().call();
	}
}}
