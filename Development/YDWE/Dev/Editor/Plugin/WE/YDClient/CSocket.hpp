#pragma once

#include <winsock.h>
#include "CLock.hpp"

class CSocket
{
public:
    CSocket();
    ~CSocket();

	bool init();
    bool open();
    bool close();
    int  recvfrom(PBYTE szBuf, SOCKADDR_IN* from);	
	int  sendto(PBYTE pData, int nSize);
private:
    CFastLock m_cLock;
    WSADATA   m_wsaData;
	SOCKET    m_Socket;
	UINT16    m_nPort;
    UINT16    m_nDstPort;
	IN_ADDR   m_nDstAddr;
};
