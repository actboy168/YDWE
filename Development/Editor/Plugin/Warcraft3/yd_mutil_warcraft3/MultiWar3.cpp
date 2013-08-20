#include <WinSock.h>
#include <ydwe/hook/iat_manager.h>
#include <ydwe/util/foreach.h>
#include <ydwe/warcraft3/version.h>
#include <ydwe/warcraft3/war3_searcher.h>
#include <aero/function/fp_call.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp> 
#include <boost/interprocess/containers/set.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp> 
#include <functional>

#pragma comment(lib, "wsock32.lib")

class udp_port_manager
{
	typedef boost::interprocess::allocator<uint16_t, boost::interprocess::managed_shared_memory::segment_manager> shm_uint16_allocator; 
	typedef boost::interprocess::set<uint16_t, std::less<uint16_t>, shm_uint16_allocator> shm_set; 

public:
	udp_port_manager()
		: shm_(boost::interprocess::open_or_create, "ydwe.warcraft3.port_manager", 1024)
		, named_mtx_(boost::interprocess::open_or_create, "ydwe.warcraft3.port_manager.mutex")
		, port_set_(shm_.find_or_construct<shm_set>("port_set")(std::less<uint16_t>(), shm_.get_segment_manager()))
		, self_port_(0)
		, self_socket_(INVALID_SOCKET)
	{ }

    void each_other(std::function<void(uint16_t)> func)
	{
		boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(named_mtx_);
        foreach (uint16_t port, *port_set_)
        {
			if (port != self_port_)
			{
				func(port);
			}
        }
	}

	void set_self(SOCKET s, uint16_t port)
	{
		self_socket_ = s;
		self_port_   = port;

		boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(named_mtx_);
		port_set_->insert(self_port_);
	}

	void remove(SOCKET s)
	{
		if (self_socket_ == s && self_port_)
		{
			boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(named_mtx_);
			port_set_->erase(self_port_);
		}
	}

	boost::interprocess::managed_shared_memory shm_; 
	boost::interprocess::named_mutex           named_mtx_;
	shm_set*                                   port_set_; 
	uint16_t                                   self_port_;
	SOCKET                                     self_socket_;
};

std::unique_ptr<udp_port_manager> g_udp_port_manager;
uint16_t g_tcp_port;

uintptr_t real_bind;
uintptr_t real_closesocket;
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
				uint16_t udp_port  = ::ntohs(((struct sockaddr_in*)name)->sin_port);

				for (;;)
				{
					if (aero::std_call<int>(real_bind, s, name, namelen) != SOCKET_ERROR)
					{
						g_udp_port_manager->set_self(s, udp_port);
						return 0;
					}

					((struct sockaddr_in*)name)->sin_port = ::htons(++udp_port);
				}
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
				if (aero::std_call<int>(real_bind, s, name, namelen) != SOCKET_ERROR)
				{
					g_tcp_port = port;
					return 0;
				}

				((struct sockaddr_in*)name)->sin_port = ::htons(++port);
			}
		}
    }

    return aero::std_call<int>(real_bind, s, name, namelen);
}

int __stdcall fake_closesocket(SOCKET s)
{
	g_udp_port_manager->remove(s);

    return aero::std_call<int>(real_closesocket, s);
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

    if (data_ptr->f7_ == 0xF7)
    {
        if (data_ptr->cmd_ == 0x30)
		{
			*(uint16_t*)(&buf[data_ptr->size_-2]) = g_tcp_port;
        }
		
		struct sockaddr_in* info_ptr = (struct sockaddr_in*)to;

        if (info_ptr->sin_port == ::htons(6112) && info_ptr->sin_addr.S_un.S_addr == ::inet_addr("255.255.255.255"))
		{
			g_udp_port_manager->each_other([&](uint16_t port)
			{
				if (port != 6112)
				{
					sockaddr_in sock_addr = *(sockaddr_in*)to;
					sock_addr.sin_port = ::htons(port);
					sock_addr.sin_addr.S_un.S_addr = ::inet_addr("127.0.0.1");
					aero::std_call<int>(real_sendto, s, (char*)buf, len, flags, (const sockaddr*)&sock_addr, sizeof(sockaddr_in));
				}
			});
        }
    }

    return aero::std_call<int>(real_sendto, s, buf, len, flags, to, tolen);
}

HANDLE __stdcall fake_CreateEventA(LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName)
{
    HANDLE retval = aero::std_call<HANDLE>(real_CreateEventA, lpEventAttributes, bManualReset, bInitialState, lpName);

    if ((lpName != NULL) && (0 == strcmp(lpName, "Warcraft III Game Application")))
    {
        ::SetLastError(0);
    }

    return retval;
}

ydwe::hook::iat_manager g_iat;

void InitializeMutilWar3()
{
	try {
		g_udp_port_manager.reset(new udp_port_manager());

		HMODULE h = NULL;
		if (ydwe::warcraft3::get_war3_searcher().get_version() > ydwe::warcraft3::version_121b)
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
				if (g_iat.open_dll("wsock32.dll"))
				{
					g_iat.hook((const char*)2,  &real_bind,         (uintptr_t)fake_bind);
					g_iat.hook((const char*)3,  &real_closesocket,  (uintptr_t)fake_closesocket);
					g_iat.hook((const char*)20, &real_sendto,       (uintptr_t)fake_sendto);
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
