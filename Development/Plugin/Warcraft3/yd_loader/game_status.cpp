#include <Winsock2.h>
#include "DllModule.h"
#include <base/hook/fp_call.h>
#include <base/hook/inline.h>
#include <base/hook/iat.h>
#include <base/util/buffer.h>
#include <base/util/unicode.h>
#include <base/path/helper.h>
#include <deque>	
#include "game_status.h"
#include "war3_packet.h"

namespace auto_enter { namespace game_status {

	std::wstring     map_path;
	bool             open_map_flag     = false;
	bool             enable_auto_enter = true;
	WARCRAFT3_STATUS status            = WARCRAFT3_NONE_STATUS;

	void set_status(WARCRAFT3_STATUS war3_status)
	{
		switch (war3_status)
		{
		case WARCRAFT3_NONE_STATUS:
			{
				if (g_DllMod.hWar3Wnd)
				{
					status = WARCRAFT3_NONE_STATUS;
				}
				else
				{
					status = WARCRAFT3_NOWINDOW_STATUS;
				}
			}
			break;
		case WARCRAFT3_WAIT0_STATUS:
			{
				if (status >= WARCRAFT3_JOIN_STATUS)
				{
					enable_auto_enter = false;
				}
				status = WARCRAFT3_WAIT0_STATUS;
			}
			break;
		case WARCRAFT3_WAIT1_STATUS:
			{
				if (status == WARCRAFT3_WAIT0_STATUS)
				{
					status = WARCRAFT3_WAIT1_STATUS;
				}
			}
			break;
		case WARCRAFT3_JOIN_STATUS:
			{
				if (status == WARCRAFT3_WAIT1_STATUS)
				{
					status= WARCRAFT3_JOIN_STATUS;
				}
			}
			break;
		case WARCRAFT3_CREATE_STATUS:
			{
				status = WARCRAFT3_CREATE_STATUS;
			}
			break;
		default:
			break;
		}
	}

	void udp_recv_game_info(const char* buf, int len)
	{
		try
		{
			using namespace base;

			buffer b1(buf, len);
			buffer_reader reader1(b1);

			reader1.read<uint32_t>();
			reader1.read<uint32_t>();
			reader1.read<uint32_t>();
			reader1.read<uint32_t>();
			reader1.read<std::string>();
			reader1.read<uint8_t>();

			buffer b2 = packet_decode<buffer>(reader1.read<std::string>());
			buffer_reader reader2(b2);

			reader2.read<uint32_t>();
			reader2.read<uint8_t>();
			reader2.read<uint16_t>();
			reader2.read<uint16_t>();
			reader2.read<uint32_t>();
			map_path = u2w(reader2.read<std::string>());
		}
		catch (...)
		{
		}
	}

	void udp_send(const char* buf, int len)
	{
		war3_packet* ptr = (war3_packet*)buf;
		if (!ptr->vaild(len)) return ;

		switch (ptr->type)
		{
		case UDP_GAME_SEARCH:
			//表示自己开始搜索主机了
			set_status(WARCRAFT3_WAIT0_STATUS);
			open_map_flag = false;
			break;
		case UDP_GAME_CREATE:
			//表示自己创建主机了
			set_status(WARCRAFT3_CREATE_STATUS);
			break;
		case UDP_GAME_CANCEL:
			//表示自己把创建的主机取消掉了
			set_status(WARCRAFT3_WAIT0_STATUS);
			break;
		case UDP_GAME_INFO:
			break;
		case UDP_GAME_UPDATE:
			break;
		default:
			break;
		}
	}

	void udp_recv(const char* buf, int len)
	{
		war3_packet* ptr = (war3_packet*)buf;
		if (!ptr->vaild(len)) return ;

		switch (ptr->type)
		{
		case UDP_GAME_SEARCH:
			break;
		case UDP_GAME_CREATE:
			break;
		case UDP_GAME_CANCEL:
			break;
		case UDP_GAME_INFO:
			udp_recv_game_info(buf + sizeof (war3_packet), len - sizeof (war3_packet));
			set_status(WARCRAFT3_WAIT1_STATUS);
			break;
		case UDP_GAME_UPDATE:
			break;
		default:
			break;
		}
	}

	uintptr_t real_sendto      = (uintptr_t)::GetProcAddress(::LoadLibraryW(L"ws2_32.dll"), "sendto");
	uintptr_t real_connect     = (uintptr_t)::GetProcAddress(::LoadLibraryW(L"ws2_32.dll"), "connect");
	uintptr_t real_recvfrom    = 0;

	int __stdcall fake_sendto(SOCKET s, const char* buf, int len, int flags, const struct sockaddr* to, int tolen)
	{
		udp_send(buf, len);
		return base::std_call<int>(real_sendto, s, buf, len, flags, to, tolen);
	}

	int __stdcall fake_recvfrom(SOCKET s, char* buf, int len, int flags, struct sockaddr* from, int* fromlen)
	{
		int ret = base::std_call<int>(real_recvfrom, s, buf, len, flags, from, fromlen);
		if (ret != SOCKET_ERROR)
		{
			udp_recv(buf, ret);
		}
		return ret;
	}

	int __stdcall fake_connect(SOCKET s, const struct sockaddr* name, int namelen)
	{
		set_status(WARCRAFT3_JOIN_STATUS);
		return base::std_call<int>(real_connect, s, name, namelen);
	}

	uintptr_t real_sfile_open_archive    = 0;
	uintptr_t real_sfile_open_archive_ex = 0;

	void sfile_open_archive_fitler(const char* archive_name)
	{
		if (!open_map_flag && archive_name && !map_path.empty())
		{
			try {
				if (base::path::equal(fs::path(map_path).filename(), fs::path(archive_name).filename()))
				{
					open_map_flag = true;
				}
			}
			catch (...) {				
			}
		}
	}

	bool __stdcall fake_sfile_open_archive(const char* archive_name, uint32_t priority, uint32_t flags, HANDLE* handle_ptr)
	{
		if (IsBadReadPtr(archive_name, 1))
		{
			return false;
		}

		sfile_open_archive_fitler(archive_name);
		return base::std_call<bool>(real_sfile_open_archive, archive_name, priority, flags, handle_ptr);
	}

	bool __stdcall fake_sfile_open_archive_ex(HANDLE mpq_handle, const char* archive_name, uint32_t priority, uint32_t flags, HANDLE* handle_ptr)
	{
		if (IsBadReadPtr(archive_name, 1))
		{
			return false;
		}

		sfile_open_archive_fitler(archive_name);
		return base::std_call<bool>(real_sfile_open_archive_ex, mpq_handle, archive_name, priority, flags, handle_ptr);
	}

	void initialize(HMODULE gamedll)
	{
		real_connect  = base::hook::iat(gamedll, "wsock32.dll", (const char*)4 /*"connect"*/,  (uintptr_t)fake_connect);
		real_recvfrom = base::hook::iat(gamedll, "wsock32.dll", (const char*)17/*"recvfrom"*/, (uintptr_t)fake_recvfrom);
		real_sendto   = base::hook::iat(gamedll, "wsock32.dll", (const char*)20/*"sendto"*/,   (uintptr_t)fake_sendto);

		real_sfile_open_archive    = base::hook::iat(gamedll, "Storm.dll", (const char*)266, (uintptr_t)fake_sfile_open_archive);
		real_sfile_open_archive_ex = base::hook::iat(gamedll, "Storm.dll", (const char*)293, (uintptr_t)fake_sfile_open_archive_ex);
	}
}}
