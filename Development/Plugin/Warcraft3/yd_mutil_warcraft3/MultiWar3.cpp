#include <WinSock.h>
#include <base/hook/iat_manager.h>
#include <base/util/foreach.h>
#include <base/warcraft3/version.h>
#include <base/warcraft3/war3_searcher.h>
#include <base/hook/fp_call.h>
#include <functional>  

#pragma comment(lib, "wsock32.lib")


uint16_t g_tcp_port;

uintptr_t real_bind;
uintptr_t real_sendto;
uintptr_t real_CreateEventA;

int __stdcall fake_bind(SOCKET s, const struct sockaddr FAR* name, int namelen)
{
    int optVal;
    int optLen = sizeof(int);

    if (SOCKET_ERROR != ::getsockopt(s, SOL_SOCKET, SO_TYPE, (char*)&optVal, &optLen))
    {
        if (optVal == SOCK_DGRAM)
		{
			if (((struct sockaddr_in*)name)->sin_port == 0xE017)
			{
				// udp && port == 6112
				BOOL optval = TRUE;
				::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));
				return base::std_call<int>(real_bind, s, name, namelen);
			}
		}
		else if (optVal == SOCK_STREAM)
		{
			// tcp
			BOOL optval = FALSE;
			::setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));

			uint16_t port = ::ntohs(((struct sockaddr_in*)name)->sin_port);

			for (;;)
			{
				if (base::std_call<int>(real_bind, s, name, namelen) != SOCKET_ERROR)
				{
					g_tcp_port = port;
					return 0;
				}

				((struct sockaddr_in*)name)->sin_port = ::htons(++port);
			}
		}
    }

	return base::std_call<int>(real_bind, s, name, namelen);
}

int __stdcall fake_sendto(SOCKET s, const char FAR * buf, int len, int flags, const struct sockaddr FAR * to, int tolen)
{
	struct war3_packet
	{
		uint8_t  f7_;
		uint8_t  cmd_;
		uint16_t size_;
	};

    struct war3_packet* data_ptr = (struct war3_packet*)buf;

	if (len >= sizeof(war3_packet) && data_ptr->f7_ == 0xF7)
    {
        if (data_ptr->cmd_ == 0x30)
		{
			*(uint16_t*)(&buf[data_ptr->size_ - 2]) = g_tcp_port;
			sockaddr_in addr = *(sockaddr_in*)to;
			addr.sin_addr.S_un.S_addr = ::inet_addr("255.255.255.255");
			return base::std_call<int>(real_sendto, s, (char*)buf, len, flags, (const sockaddr*)&addr, sizeof(sockaddr_in));
        }
    }

	return base::std_call<int>(real_sendto, s, buf, len, flags, to, tolen);
}

HANDLE __stdcall fake_CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
	HANDLE retval = base::std_call<HANDLE>(real_CreateEventA, lpEventAttributes, bManualReset, bInitialState, lpName);

    if ((lpName != NULL) && (0 == strcmp(lpName, "Warcraft III Game Application")))
    {
        ::SetLastError(0);
    }

    return retval;
}

base::hook::iat_manager g_iat;

void InitializeMutilWar3()
{
	try { 
		HMODULE h = NULL;
		if (base::warcraft3::get_war3_searcher().get_version() > base::warcraft3::version_121b)
		{
			h = ::GetModuleHandleW(L"Game.dll");
		}
		else
		{
			h = ::GetModuleHandleW(L"War3.exe");
		}

		if (h)
		{
			if (g_iat.open_module(h))
			{
				if (base::warcraft3::get_war3_searcher().get_version() >= base::warcraft3::version_127a)
				{
					if (g_iat.open_dll("ws2_32.dll"))
					{
						g_iat.hook((const char*)2, &real_bind, (uintptr_t)fake_bind);
						g_iat.hook((const char*)20, &real_sendto, (uintptr_t)fake_sendto);
					}
				}
				else
				{
					if (g_iat.open_dll("wsock32.dll"))
					{
						g_iat.hook((const char*)2, &real_bind, (uintptr_t)fake_bind);
						g_iat.hook((const char*)20, &real_sendto, (uintptr_t)fake_sendto);
					}
				}

				if (g_iat.open_dll("kernel32.dll"))
				{
					g_iat.hook("CreateEventA",  &real_CreateEventA, (uintptr_t)fake_CreateEventA);
				}
			}
		}
	}
	catch (...) {
	}
}
