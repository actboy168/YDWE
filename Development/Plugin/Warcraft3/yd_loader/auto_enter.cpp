#include "DllModule.h"
#include "auto_enter.h"
#include "game_status.h"
#include <cstdint>
#include <thread>

namespace auto_enter {

	void initialize()
	{
		std::thread([]()
		{
			for (; game_status::enable_auto_enter; std::this_thread::sleep_for(std::chrono::milliseconds(300)))
			{
				if (game_status::status == game_status::WARCRAFT3_NONE_STATUS)
				{
					::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, 'L', 0);
					::SendMessage(g_DllMod.hWar3Wnd, WM_KEYUP, 'L', 0);
					//::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, VK_RETURN, 0);
					//::SendMessage(g_DllMod.hWar3Wnd, WM_KEYUP,   VK_RETURN, 0);
				}

				if (game_status::status == game_status::WARCRAFT3_WAIT1_STATUS)
				{
					::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, VK_TAB, 0);
					::SendMessage(g_DllMod.hWar3Wnd, WM_KEYUP, VK_TAB, 0);
					::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, VK_TAB, 0);
					::SendMessage(g_DllMod.hWar3Wnd, WM_KEYUP, VK_TAB, 0);
					::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, VK_TAB, 0);
					::SendMessage(g_DllMod.hWar3Wnd, WM_KEYUP, VK_TAB, 0);
					::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, VK_TAB, 0);
					::SendMessage(g_DllMod.hWar3Wnd, WM_KEYUP, VK_TAB, 0);

					if (game_status::open_map_flag)
					{
						::SendMessage(g_DllMod.hWar3Wnd, WM_KEYDOWN, 'J', 0);
						::SendMessage(g_DllMod.hWar3Wnd, WM_KEYUP, 'J', 0);
					}
				}

				if (game_status::status == game_status::WARCRAFT3_JOIN_STATUS)
				{
					::ShowWindow(g_DllMod.hWar3Wnd, SW_SHOW);
				}
			}
		});
	}
}
