#pragma once

#include <base/util/noncopyable.h>
#include <Windows.h>
#include <intrin.h>

#pragma intrinsic( _InterlockedExchange8 )
#pragma intrinsic( _ReadWriteBarrier )

namespace base { namespace thread { namespace lock {
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
				SYSTEM_INFO info;
				::GetSystemInfo(&info);
				enable = (info.dwNumberOfProcessors > 1);
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
				::Sleep(1);
				return;
			}

			if (exp_cur_ <= exp_max_)
			{
				for (size_t n = 0; n < exp_cur_; ++n)
				{
					__nop();
				}
				exp_cur_ *= 2;
			}
			else
			{
				if (((exp_cur_ - exp_max_) % kSleep1EveryHowManyTimes) == (kSleep1EveryHowManyTimes - 1))
				{
					::Sleep(1);
				}
				else
				{
					::SwitchToThread();
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

	template <class BackoffT = back_off<0>>
	class spin
		: private util::noncopyable
	{
	public:
		inline spin()
			: spin_(false)
		{ }

		inline spin(bool locked)
			: spin_(locked)
		{ }

		inline ~spin()
		{ }

		inline bool lockable() const
		{
			return !spin_;
		}

		inline bool try_lock()
		{
#pragma warning(suppress:4800)
			return !(bool)_InterlockedExchange8(reinterpret_cast<char volatile *>(&spin_), (char)true);
		}

		inline void unlock()
		{
			spin_ = false;
			_ReadWriteBarrier();
		}

		inline void lock()
		{
			for (BackoffT backoff;; backoff())
			{
				if (lockable() && try_lock())
				{
					break;
				}
			}
		}

	private:
		volatile bool        spin_;
	};
}}}
