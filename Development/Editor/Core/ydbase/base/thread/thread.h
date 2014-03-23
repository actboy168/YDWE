#pragma once

#include <base/config.h>
#include <Windows.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <base/thread/intrusive_ptr.h>
#include <base/thread/thread_exception.h>

namespace base
{
	struct _BASE_API thread_data_base
	{
		long     count;
		HANDLE   thread_handle;
		unsigned thread_id;

		thread_data_base();
		virtual ~thread_data_base();
		virtual void run() = 0;

		friend void intrusive_ptr_add_ref(thread_data_base* p)
		{
			::InterlockedIncrement(&p->count);
		}

		friend void intrusive_ptr_release(thread_data_base* p)
		{
			if (!InterlockedDecrement(&p->count))
			{
				delete p;
			}
		}
	};

	template <typename F>
	class thread_data : public thread_data_base
	{
	public:
		thread_data(F f_) : f(f_) { }
		void run() { f(); }
	private:
		F f;
	};

	namespace this_thread
	{
		_BASE_API void sleep_for(uintmax_t target_time);
	}

	class _BASE_API thread
	{
	public:
		typedef unsigned id;

		thread();

		template <class C1>
		explicit thread(C1 c1)
			: thread_info(make_thread_data(c1))
		{
			start_thread();
		}

		template <class C1, class C2>
		explicit thread(C1 c1, C2 c2)
			: thread_info(make_thread_data(std::bind(c1, c2)))
		{
			start_thread();
		}

		~thread();
		void       detach();
		void       join();
		thread::id get_id() const;

	private:
		void start_thread();

		template <typename F>
		thread_data<F>* make_thread_data(F f)
		{
			return new thread_data<F>(f);
		}

#pragma warning(suppress:4251)
		intrusive_ptr<thread_data_base> thread_info;
	};
}
