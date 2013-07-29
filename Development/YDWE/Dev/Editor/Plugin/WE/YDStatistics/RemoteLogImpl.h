#pragma once

#include <curl/curl.h>
#include <curl/easy.h>

#include <map>
#include <list>
#include <string>
#include "Lock.h"
#include "Event.h"
#include "Thread.h"

typedef std::map<std::string, std::string> PostData;
typedef std::string                        PostType;

class RemoteLogImpl: public Runnable
{
public:
	RemoteLogImpl();
	~RemoteLogImpl();
	void start();
	void stop(long milliseconds = 300);
	void post_to_server(PostType* pTableName, PostData* pData);

private:
	void run();
	void send_to_server(PostData& data);

private:
	static size_t write_function(void* ptr, size_t size, size_t nmemb, void* stream);

private:
	Thread               _thread;
	Event                _stopped;
	CURL*                _curl;
	FastLock             _lock;
	std::list<PostData>  _post_queue;
};

