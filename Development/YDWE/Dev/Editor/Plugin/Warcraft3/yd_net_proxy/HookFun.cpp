#include <ydwe/hook/inline.h>
#include <aero/function/fp_call.hpp>
#include <winsock2.h>
#include <cstdint>
#include <memory>

#pragma comment(lib, "ws2_32.lib")

struct PACKETHEADER
{
	uint16_t	nSize;			// 包头+数据长度
	uint16_t	nType;			// 包类型
	uint32_t	nDestId;		// 目的主机Id
	uint32_t	nSrcId;			// 原主机Id
	uint16_t	nDestGamePort;	// 目的主机的本地游戏端口
	uint16_t	nSrcGamePort;	// 原主机游戏端口号	
};

#define		PACKET_UDP_FROMLOCAL			0x1001	//UDP本机数据发到对方
#define		PACKET_BROADCAST_FROMLOCAL		0x1002	//UDP广播数据发到VPNSERVER
#define		PACKET_TCP_CONNECT				0x1006	//TCP连接
#define		PACKET_UDP_FROMREMOTE			0x1007	//对方发送的主机包，发送到AS

struct WAR3_PACKETHEADER
{
	uint8_t	 nReserve;		//保留0xF7
	uint8_t	 nCmd;			//命令字
	uint16_t nSize;			//包大小
};

struct LocalInfo
{
	uint32_t		m_nHostId;			// 主机ID
	uint16_t		m_nUdpProxyPort;	//本机Udp代理端口
	uint16_t		m_nTcpProxyPort;	//本机TCP代理端口
};

LocalInfo		g_LocalInfo = {0};

uintptr_t real_bind        = (uintptr_t)::GetProcAddress(::LoadLibraryW(L"ws2_32.dll"), "bind");
uintptr_t real_connect     = (uintptr_t)::GetProcAddress(::LoadLibraryW(L"ws2_32.dll"), "connect");
uintptr_t real_recvfrom    = (uintptr_t)::GetProcAddress(::LoadLibraryW(L"ws2_32.dll"), "recvfrom");
uintptr_t real_sendto      = (uintptr_t)::GetProcAddress(::LoadLibraryW(L"ws2_32.dll"), "sendto");
uintptr_t real_wsarecvfrom = (uintptr_t)::GetProcAddress(::LoadLibraryW(L"ws2_32.dll"), "WSARecvFrom");

namespace sockopt
{
	int get(SOCKET s, int name)
	{
		int optval = 0;
		int optlen = sizeof(int);
		if (SOCKET_ERROR == ::getsockopt(s, SOL_SOCKET, name, (char*)&optval, &optlen))
		{
			return SOCKET_ERROR;
		}

		return optval;
	}

	int set(SOCKET s, int name, int optval)
	{
		int optlen = sizeof(int);
		if (SOCKET_ERROR == ::setsockopt(s, SOL_SOCKET, name, (char*)&optval, optlen))
		{
			return SOCKET_ERROR;
		}

		return 0;
	}

	class guard
	{
	public:
		guard(SOCKET s, int name, int optval)
			: s_(s)
			, name_(name)
			, origval_(get(s, name))
		{
			set(s_, name_, optval);
		}

		~guard()
		{
			if (origval_ != SOCKET_ERROR)
			{
				set(s_, name_, origval_);
			}
		}

	private:
		SOCKET s_;
		int name_;
		int origval_;
	};
}

int __stdcall fake_bind(SOCKET s, const struct sockaddr* name, int namelen)
{
	if (name == NULL) 
		return SOCKET_ERROR;

	if (::htons(((sockaddr_in*)name)->sin_port) != 6112)
	{
		return aero::std_call<int>(real_bind, s, name, namelen);
	}

	sockaddr_in addr = *(sockaddr_in*)name;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;

	for (;; addr.sin_port = ::htons(::ntohs(addr.sin_port) + 1))
	{
		if (SOCKET_ERROR != aero::std_call<int>(real_bind, s, (sockaddr*)&addr, namelen))
		{
			break;
		}
	}

	return 0;
}

int recvfrom_remote(SOCKET s, char* buf, int len, sockaddr_in* from)
{
	if (len > 0 && ::htons(from->sin_port) == g_LocalInfo.m_nUdpProxyPort)
	{
		PACKETHEADER* pHeader=(PACKETHEADER*)buf;
		if (pHeader->nType != PACKET_UDP_FROMREMOTE)
		{
			return len;
		}

		from->sin_family = AF_INET;
		from->sin_addr.S_un.S_addr = ::htonl(pHeader->nSrcId);
		from->sin_port = ::htons(pHeader->nSrcGamePort);

		len -= sizeof(PACKETHEADER);
		memmove(buf, buf + sizeof(PACKETHEADER), len);
	}
	return len;
}

int __stdcall fake_wsarecvfrom(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr* lpFrom, LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped, LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
	int	ret = aero::std_call<int>(real_wsarecvfrom, s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpFrom, lpFromlen, NULL, NULL);
	if (ret == SOCKET_ERROR)
		return SOCKET_ERROR;

	*lpNumberOfBytesRecvd = recvfrom_remote(s, (char*)lpBuffers->buf, *lpNumberOfBytesRecvd, (SOCKADDR_IN*)lpFrom);
	return 0;
}

int __stdcall fake_recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen)
{
	int ret = aero::std_call<int>(real_recvfrom, s, buf, len, flags, from, fromlen);
	if (ret == SOCKET_ERROR)
		return SOCKET_ERROR;

	ret = recvfrom_remote(s, (char*)buf, ret, (sockaddr_in*)from);
	*fromlen = sizeof(sockaddr_in);
	return ret;
}

int sendto_proxy(SOCKET s, const char* buf, int len, PACKETHEADER& stHeader)
{
	std::unique_ptr<char[]> buffer(new char[sizeof(PACKETHEADER) + len]);
	stHeader.nSize = sizeof(PACKETHEADER) + len;	
	memcpy(buffer.get(), &stHeader, sizeof(PACKETHEADER));
	if (buf != NULL && len > 0)
	{
		memcpy(buffer.get() + sizeof(PACKETHEADER), buf, len);
	}

	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = ::htons(g_LocalInfo.m_nUdpProxyPort);
	int addrlen   = sizeof(sockaddr_in);
	int nSendBytes = aero::std_call<int>(real_sendto, s, buffer.get(), stHeader.nSize, 0, (sockaddr*)&addr, addrlen);
	return len;
}

int __stdcall fake_sendto (SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)
{
	if (to == NULL || tolen <= 0) 
		return SOCKET_ERROR;

	if (((const WAR3_PACKETHEADER*)buf)->nReserve != 0xF7 || ((const WAR3_PACKETHEADER*)buf)->nSize != len)
	{
		return aero::std_call<int>(real_sendto, s, buf, len, 0, to, tolen);
	}

	PACKETHEADER stHeader  = {0};
	stHeader.nSrcGamePort  = 6112;
	stHeader.nSrcId        = g_LocalInfo.m_nHostId;
	stHeader.nDestGamePort = ::ntohs(((sockaddr_in*)to)->sin_port);

	if (::inet_addr("255.255.255.255") == ((sockaddr_in*)to)->sin_addr.S_un.S_addr)
	{
		stHeader.nType   = PACKET_BROADCAST_FROMLOCAL;
		sockopt::guard g(s, SO_BROADCAST, FALSE);
		sendto_proxy(s, buf, len, stHeader);
	}
	else
	{
		stHeader.nDestId = ::htonl(((sockaddr_in*)to)->sin_addr.S_un.S_addr);
		stHeader.nType   = PACKET_UDP_FROMLOCAL;
		sendto_proxy(s, buf, len, stHeader);
	}

	return len;
}

int is_connected(SOCKET s)
{
	fd_set fd_write;
	FD_ZERO(&fd_write);
	timeval tv = {0};
	tv.tv_usec= 100000;
	FD_SET(s, &fd_write);
	if (::select(0, NULL, &fd_write, NULL, &tv) == SOCKET_ERROR)
		return SOCKET_ERROR;
	if (FD_ISSET(s, &fd_write) == 0)
		return SOCKET_ERROR;
	return 0;
}

int send_connect_packet(SOCKET s, uint32_t nDestId, uint16_t nSrcPort, uint16_t nDestPort)
{
	PACKETHEADER stHeader  = {0};
	stHeader.nType         = PACKET_TCP_CONNECT;
	stHeader.nSize         = sizeof(stHeader);
	stHeader.nDestId       = nDestId;
	stHeader.nSrcId        = g_LocalInfo.m_nHostId;
	stHeader.nSrcGamePort  = nSrcPort;
	stHeader.nDestGamePort = nDestPort;

	return ::send(s, (char*)&stHeader, sizeof(PACKETHEADER), 0);
}

int __stdcall hookconnect(SOCKET s, const struct sockaddr* name, int namelen)
{
	if (name == NULL)
		return SOCKET_ERROR;

	//这个地方的特殊处理，会导致非6112的主机没有人加入
	if (::htons(((sockaddr_in*)name)->sin_port) != 6112)
	{
		return aero::std_call<int>(real_connect, s, name, namelen);		
	}

	sockaddr_in addr = {0};
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = ::htons(g_LocalInfo.m_nTcpProxyPort);
	addr.sin_family= AF_INET;
	int ret = aero::std_call<int>(real_connect, s, (sockaddr*)&addr, sizeof(sockaddr_in));
	if (ret != 0 && ::WSAGetLastError() == WSAEWOULDBLOCK)
	{
		for (int count = 0; count < 30; ++count)
		{
			if (is_connected(s) == 0)
			{
				ret = 0;
				break;
			}
		}
	}

	if (ret == 0)
	{
		send_connect_packet(s, ::htonl(((sockaddr_in*)name)->sin_addr.S_un.S_addr), 0, 6112);
	}

	return ret;
}

void SearchProxyPort(uint16_t& udp_proxy_port, uint16_t& tcp_proxy_port);

void HookAttach()
{
	ydwe::hook::inline_install(&real_bind,	      (uintptr_t)fake_bind);
	ydwe::hook::inline_install(&real_connect,	  (uintptr_t)hookconnect);
	ydwe::hook::inline_install(&real_sendto,	  (uintptr_t)fake_sendto);
	ydwe::hook::inline_install(&real_recvfrom,	  (uintptr_t)fake_recvfrom);
	ydwe::hook::inline_install(&real_wsarecvfrom, (uintptr_t)fake_wsarecvfrom);
	
	g_LocalInfo.m_nHostId = inet_addr("83.76.228.17");//300174419;
	SearchProxyPort(g_LocalInfo.m_nUdpProxyPort, g_LocalInfo.m_nTcpProxyPort);
}

void HookDetach()
{	
	ydwe::hook::inline_uninstall(&real_bind,	    (uintptr_t)fake_bind);	
	ydwe::hook::inline_uninstall(&real_connect,		(uintptr_t)hookconnect);
	ydwe::hook::inline_uninstall(&real_sendto,		(uintptr_t)fake_sendto);
	ydwe::hook::inline_uninstall(&real_recvfrom,	(uintptr_t)fake_recvfrom);
	ydwe::hook::inline_uninstall(&real_wsarecvfrom,	(uintptr_t)fake_wsarecvfrom);
}
