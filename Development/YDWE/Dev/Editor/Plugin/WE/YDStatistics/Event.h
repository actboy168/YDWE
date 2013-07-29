#pragma once

class Event
{
public:
	Event(bool autoReset = true)
	{
		_event = CreateEventW(NULL, autoReset ? FALSE : TRUE, FALSE, NULL);
		if (!_event)
		{
			// cannot create event
			return ;
		}
	}

	~Event()
	{
		CloseHandle(_event);
	}

	void set()
	{
		if (!SetEvent(_event))
		{
			// cannot signal event
			return ;
		}
	}

	void wait()
	{
		switch (WaitForSingleObject(_event, INFINITE))
		{
		case WAIT_OBJECT_0:
			return;
		default:
			// wait for event failed
			return ;
		}
	}

	bool wait(long milliseconds)
	{
		switch (WaitForSingleObject(_event, milliseconds))
		{
		case WAIT_TIMEOUT:
			return false;
		case WAIT_OBJECT_0:
			return true;
		default:
			// wait for event failed
			return false;	
		}
	}

	void reset()
	{
		if (!ResetEvent(_event))
		{
			// cannot reset event
			return ;
		}
	}

private:
	Event(const Event&);
	Event& operator = (const Event&);
private:
	HANDLE _event;
};
