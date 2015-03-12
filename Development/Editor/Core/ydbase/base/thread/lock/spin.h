#pragma once

#include <base/util/noncopyable.h>
#include <base/thread/lock/backoff.h>

namespace base { namespace lock {

	template <class BackoffT = back_off<0>>
	class spin
		: private noncopyable
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
#if defined(WIN32)
#pragma warning(suppress:4800)
			return !(bool)_InterlockedExchange8(reinterpret_cast<char volatile *>(&spin_), (char)true);
#else
			return !(bool)__sync_lock_test_and_set(&spin_, (char)true);
#endif
		}

		inline void unlock()
		{
			spin_ = false;
			COMPILER_RW_BARRIER;
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
}}
