#include "stdafx.h"
#include "CSocket.hpp"

#pragma comment(lib, "wsock32.lib")

CSocket::CSocket()
	: m_Socket(INVALID_SOCKET), m_nPort(12000), m_nDstPort(5166)
{
	memset(&m_nDstAddr, 0, sizeof(m_nDstAddr));
}

CSocket::~CSocket() 
{
	WSACleanup();
}

bool CSocket::init()
{
	if (0 != WSAStartup(MAKEWORD(1, 1), &m_wsaData))
	{
		return false;
	}

	struct hostent* phe = ::gethostbyname("s.ydwe.net");
	if (NULL == phe)
	{
		return false;
	}

	m_nDstAddr = *(LPIN_ADDR)*(phe->h_addr_list);

	return true;
}

bool CSocket::open()
{
    AUTO_LOCK(m_cLock);

	if (!init())
	{
		return false;
	}

    m_Socket = INVALID_SOCKET;

    m_Socket = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (m_Socket == SOCKET_ERROR)
    {
		return false;
	}
	m_nPort = 12000;

	while (m_nPort < (UINT16)-1)
	{
		SOCKADDR_IN sockAddr = {0};
		sockAddr.sin_family  = AF_INET;
		sockAddr.sin_port    = ::htons(m_nPort);
		sockAddr.sin_addr.S_un.S_addr = INADDR_ANY; 

		if (::bind(m_Socket, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR_IN)) != SOCKET_ERROR)
		{
			break;
		}
		m_nPort++;
	}

	if (m_nPort == (UINT16)-1)
	{
		::closesocket(m_Socket);
		m_Socket = INVALID_SOCKET;
		return false;
	}

	int nBuf =	128*1024;
	::setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (char*)&nBuf, sizeof(nBuf)); 
	::setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (char*)&nBuf, sizeof(nBuf)); 
	u_long timeout = 3;
	::ioctlsocket(m_Socket, FIONBIO, &timeout);
	return true;
}

bool CSocket::close()
{
    AUTO_LOCK(m_cLock);

    bool ret = (0 == ::closesocket(m_Socket));
    m_Socket = INVALID_SOCKET;
    return ret;
}

#define MAX_UDPDATA_SIZE 1024
int CSocket::recvfrom(PBYTE szBuf, SOCKADDR_IN* from)
{
    AUTO_LOCK(m_cLock);

    int nAddrLen = sizeof(SOCKADDR_IN);
    int nSize = ::recvfrom(m_Socket, (char*)szBuf, MAX_UDPDATA_SIZE, 0, (SOCKADDR*)from, &nAddrLen);

    return nSize;
}

int CSocket::sendto(PBYTE pData, int nSize)
{
    AUTO_LOCK(m_cLock);

    SOCKADDR_IN sockAddr = {0};
    sockAddr.sin_family= AF_INET;
    sockAddr.sin_port = ::htons(m_nDstPort);
    sockAddr.sin_addr.S_un.S_addr = m_nDstAddr.S_un.S_addr;
    return ::sendto(m_Socket, (char*)pData, nSize, 0, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
}
