#pragma once

#include <map>
#include <string>

typedef std::string                        PostType;
typedef std::map<std::string, std::string> PostData;

class RemoteLogImpl;

class RemoteLog
{
public:
	RemoteLog();
	~RemoteLog();

	void start();
	void stop(long milliseconds = 300);
	void post_to_server(PostType* pTableName, PostData* pData);

private:
	RemoteLogImpl* _pImpl;
};
