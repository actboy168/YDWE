#include <base/thread/thread.h>
#include <base/win/get_tick_count.h>
#include <process.h>

namespace base
{
	thread_data_base::thread_data_base()
		: count(0)
		, thread_handle(INVALID_HANDLE_VALUE)
		, thread_id(0)
	{ }

	thread_data_base::~thread_data_base() 
	{ }

	static unsigned __stdcall thread_start_function(void* param)
	{
		thread_data_base* const thread_info(reinterpret_cast<thread_data_base*>(param));
		thread_info->run();
		return 0;
	}

	namespace this_thread
	{
		thread::id get_id()
		{
			return ::GetCurrentThreadId();
		}

		bool interruptible_wait(HANDLE handle_to_wait_for, uintmax_t target_time)
		{
			HANDLE handles[2]           = {0};
			unsigned handle_count       = 0;
			unsigned wait_handle_index  = ~0U;
			unsigned timeout_index      = ~0U;

			if (handle_to_wait_for != INVALID_HANDLE_VALUE)
			{
				wait_handle_index = handle_count;
				handles[handle_count++] = handle_to_wait_for;
			}

			HANDLE timer_handle;
			unsigned const min_timer_wait_period = 20;

			if (target_time != ~uintmax_t(0))
			{
				if (target_time > min_timer_wait_period)
				{
					timer_handle = ::CreateWaitableTimer(NULL, false, NULL);
					if (timer_handle != 0)
					{
						LARGE_INTEGER due_time = {{0}};
						due_time.QuadPart = -((LONGLONG)target_time*10000);

						bool const set_time_succeeded = ::SetWaitableTimer(timer_handle, &due_time, 0, 0, 0, false)!=0;
						if(set_time_succeeded)
						{
							timeout_index=handle_count;
							handles[handle_count++]=timer_handle;
						}
					}
				}
			}

			bool const using_timer  = timeout_index!=~0u;
			uintmax_t target = target_time + win::get_tick_count();

			for (;;)
			{
				if (handle_count)
				{
					unsigned long const notified_index = ::WaitForMultipleObjects(handle_count, handles, false, using_timer ? INFINITE: (DWORD)target_time);
					if (notified_index < handle_count)
					{
						if (notified_index == wait_handle_index)
						{
							return true;
						}
						else if (notified_index == timeout_index)
						{
							return false;
						}
					}
				}
				else
				{
					::Sleep((DWORD)target_time);
				}

				if (target <= win::get_tick_count())
				{
					return false;
				}

				target_time = target - win::get_tick_count();
			}
		}

		void sleep_for(uintmax_t target_time)
		{
			interruptible_wait(INVALID_HANDLE_VALUE, target_time);
		}
	}

	thread::thread()
	{ }

	thread::~thread()
	{
		detach();
	}

	void thread::detach()
	{
		thread_info = 0;
	}

	void thread::join() 
	{
		if (this_thread::get_id() == get_id())
		{
			throw thread_resource_error(std::errc::resource_deadlock_would_occur, "thread: trying joining itself");
		}

		intrusive_ptr<thread_data_base> local_thread_info = thread_info;
		if (!local_thread_info)
		{
			throw thread_resource_error(std::errc::invalid_argument, "thread: thread not joinable");
		}
		this_thread::interruptible_wait(local_thread_info->thread_handle, ~uintmax_t(0));
		detach();
	}

	thread::id thread::get_id() const
	{
		intrusive_ptr<thread_data_base> local_thread_info = thread_info;
		return local_thread_info? local_thread_info->thread_id: 0;
	}

	void thread::start_thread()
	{
		uintptr_t const new_thread = ::_beginthreadex(0, 0, &thread_start_function, thread_info.get(), CREATE_SUSPENDED, &thread_info->thread_id);
		if (!new_thread)
		{
			throw thread_resource_error();
		}
		intrusive_ptr_add_ref(thread_info.get());
		thread_info->thread_handle = (HANDLE)(new_thread);
		::ResumeThread(thread_info->thread_handle);
	}
}
