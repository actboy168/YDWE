
#include "RemoteLog.h"
#include "RemoteLogImpl.h"

RemoteLog::RemoteLog()
	: _pImpl(new RemoteLogImpl())
{

}

RemoteLog::~RemoteLog()
{
	if (_pImpl) delete _pImpl;
	_pImpl = nullptr;
}

void RemoteLog::start()
{
	if (_pImpl) _pImpl->start();
}

void RemoteLog::stop(long milliseconds)
{
	if (_pImpl) _pImpl->stop(milliseconds);
}

void RemoteLog::post_to_server(PostType* pTableName, PostData* pData)
{
	if (_pImpl) _pImpl->post_to_server(pTableName, pData);
}
