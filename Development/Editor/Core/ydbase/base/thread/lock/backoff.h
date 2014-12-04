#pragma once

#include <base/util/noncopyable.h>

#if defined(WIN32)
#include <base/thread/lock/os_win32.h>
#else
#include <base/thread/lock/os_linux.h>
#endif

namespace base { namespace lock {
	template <unsigned Tag>
	class back_off
	{
	public:
		static const size_t kExpMin = 16;
		static const size_t kExpMax = 16 * 1024;
		static const size_t kSleep0EveryHowManyTimes = 4;
		static const size_t kSleep1EveryHowManyTimes = 4;

	protected:
		size_t  exp_min_;
		size_t  exp_max_;
		size_t  exp_cur_;

		struct init_helper
		{
			bool enable;

			init_helper()
			{
				enable = os::getcpu() > 1;
			}
		};

		static init_helper init_helper_;

	public:
		back_off()
			: exp_min_(kExpMin)
			, exp_max_(kExpMax)
			, exp_cur_(exp_min_)
		{ }

		back_off(size_t exp_min, size_t exp_max)
			: exp_min_(exp_min)
			, exp_max_(exp_max)
			, exp_cur_(exp_min_)
		{ }

		void operator ()()
		{
			if (!init_helper_.enable)
			{
				os::sleep1();
				return;
			}

			if (exp_cur_ <= exp_max_)
			{
				for (size_t n = 0; n < exp_cur_; ++n)
				{
					os::nop();
				}
				exp_cur_ *= 2;
			}
			else
			{
				if (((exp_cur_ - exp_max_) % kSleep1EveryHowManyTimes) == (kSleep1EveryHowManyTimes - 1))
				{
					os::sleep1();
				}
				else
				{
					os::sleep0();
				}
				exp_cur_++;
			}
		}

		void reset()
		{
			exp_cur_ = exp_min_;
		}
	};

	template <unsigned Tag>
	typename back_off<Tag>::init_helper back_off<Tag>::init_helper_;
}}
