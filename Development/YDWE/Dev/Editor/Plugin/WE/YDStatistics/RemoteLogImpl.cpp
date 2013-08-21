
#include "RemoteLogImpl.h"

#pragma comment(lib, "libcurl_imp.lib")

RemoteLogImpl::RemoteLogImpl()
	: _curl(nullptr)
{
}

RemoteLogImpl::~RemoteLogImpl()
{
	stop();
}

void RemoteLogImpl::start()
{
	_thread.start(*this);
}

void RemoteLogImpl::stop(long milliseconds)
{
	_stopped.set();
	if (!_thread.join(milliseconds))
	{
		_thread.terminate();
		_thread.cleanup();
	}
}

void RemoteLogImpl::post_to_server(PostType* pTableName, PostData* pData)
{
	AUTO_LOCK(_lock);
	_post_queue.push_back(*pData);
	PostData& backData = _post_queue.back();
	backData.insert(std::make_pair("tablename", *pTableName));
	backData.insert(std::make_pair("privatekey", "a8Dsj12D2p"));
}

void RemoteLogImpl::run()
{
	_curl = curl_easy_init();

	if (_curl)
	{
		curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &write_function);
	}

	while (!_stopped.wait(0))
	{
		PostData data;

		{
			AUTO_LOCK(_lock);
			if (!_post_queue.empty())
			{
				data = _post_queue.front();
				_post_queue.pop_front();
			}
		}

		if (!data.empty())
		{
			send_to_server(data);
		}
		else
		{
			Thread::yield();
		}
	}

	if (_curl) 
	{
		curl_easy_cleanup(_curl);
	}
}

void RemoteLogImpl::send_to_server(PostData& data)
{
	if (!_curl) 
	{
		return ;
	}

	std::string post_str;
	for (auto it = data.begin(); it != data.end(); ++it)
	{
		if ((!it->first.empty()) && (!it->second.empty()))
		{
			post_str.append(it->first + "=" + it->second + "&");
		}
	}

	if (!post_str.empty())
	{
		post_str.pop_back();
		post_str = "http://www.ydwe.net/service/post.php?" + post_str;
		curl_easy_setopt(_curl, CURLOPT_URL, post_str.c_str());		
		CURLcode res = curl_easy_perform(_curl);	
	}
}

size_t RemoteLogImpl::write_function(void* ptr, size_t size, size_t nmemb, void* stream)
{
	return size * nmemb;
}
