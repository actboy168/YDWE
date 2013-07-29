#include <Windows.h>
#include <Iphlpapi.h>
#include <WinSock.h>
#include <tlhelp32.h>
#include <cstdint>
#include <functional>
#include <memory>
#pragma comment(lib, "Iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

DWORD EnumIpv4UdpPorts(std::function<void(MIB_UDPROW_OWNER_PID const&)> func)
{
	DWORD size = 0;
	DWORD error = 0;

	error = ::GetExtendedUdpTable(0, &size, TRUE, AF_INET, UDP_TABLE_OWNER_PID ,0);
	if (ERROR_INSUFFICIENT_BUFFER == error) 
	{
		std::unique_ptr<uint8_t[]> buffer(new uint8_t[size]);
		PMIB_UDPTABLE_OWNER_PID pUdpExTable = (PMIB_UDPTABLE_OWNER_PID)buffer.get();

		error = ::GetExtendedUdpTable(pUdpExTable, &size, TRUE, AF_INET, UDP_TABLE_OWNER_PID ,0); 
		if (NO_ERROR == error)
		{
			for (DWORD i = 0; i < pUdpExTable->dwNumEntries; ++i) 
			{
				func(pUdpExTable->table[i]);
			}
		}
	}

	return error;
}

DWORD EnumIpv4TcpPorts(std::function<void(MIB_TCPROW_OWNER_PID const&)> func)
{
	PMIB_TCPTABLE_OWNER_PID pTcpExTable = NULL;
	DWORD size = 0;
	DWORD error = 0;

	error = ::GetExtendedTcpTable(0, &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0);
	if (error == ERROR_INSUFFICIENT_BUFFER) 
	{
		std::unique_ptr<uint8_t[]> buffer(new uint8_t[size]);
		PMIB_TCPTABLE_OWNER_PID pTcpExTable = (PMIB_TCPTABLE_OWNER_PID)buffer.get();

		error = ::GetExtendedTcpTable(pTcpExTable, &size, TRUE, AF_INET, TCP_TABLE_OWNER_PID_ALL, 0); 
		if (NO_ERROR == error) 
		{
			for (DWORD i = 0; i < pTcpExTable->dwNumEntries; ++i) 
			{
				func(pTcpExTable->table[i]);
			}
		}
	}
	return error;
}

uint32_t GetProcessIdByName(const wchar_t* name)
{
	HANDLE hSnapShot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
		return 0;

	PROCESSENTRY32W procentry = {sizeof(PROCESSENTRY32W)};
	for (BOOL flag = ::Process32FirstW( hSnapShot, &procentry); flag; flag = ::Process32NextW(hSnapShot, &procentry))
	{
		if (_wcsicmp(procentry.szExeFile, name) == 0)
		{
			::CloseHandle(hSnapShot); 
			return procentry.th32ProcessID;
		}
	}

	::CloseHandle(hSnapShot); 
	return 0;
}

void SearchProxyPort(uint16_t& udp_proxy_port, uint16_t& tcp_proxy_port)
{
	uint16_t tcp_ctrl_port = 0;

	uint32_t target_pid = GetProcessIdByName(L"11game.exe");

	EnumIpv4TcpPorts([&](MIB_TCPROW_OWNER_PID const& table)
	{
		if (target_pid == table.dwOwningPid)
		{
			if (table.dwLocalAddr == 0 
				&& table.dwRemoteAddr == 0)
			{
				uint16_t port = ::ntohs((uint16_t)table.dwLocalPort);
				if (port >= 2100)
				{
					if (!tcp_ctrl_port)
					{
						tcp_ctrl_port = port;
					}
					else if (!tcp_proxy_port)
					{
						tcp_proxy_port = port;
					}
				}
			}	
		}
	});

	EnumIpv4UdpPorts([&](MIB_UDPROW_OWNER_PID const& table)
	{
		if (target_pid == table.dwOwningPid)
		{
			if (table.dwLocalAddr == 0)
			{
				uint16_t port = ::ntohs((uint16_t)table.dwLocalPort);
				if (port >= 2100)
				{
					if (!udp_proxy_port)
					{
						udp_proxy_port = port;
					}
				}
			}
		}
	});
}
