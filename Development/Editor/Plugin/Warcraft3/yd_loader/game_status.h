#pragma once

#include <Windows.h>

namespace auto_enter { namespace game_status {

	enum WARCRAFT3_STATUS
	{
		WARCRAFT3_NOWINDOW_STATUS = -1,     // 客户端还没有打开
		WARCRAFT3_NONE_STATUS     = 0,		// 还没有进入刷主机状态（WAR3界面已经出来了）
		WARCRAFT3_WAIT0_STATUS    = 1,		// 已经进入刷主机状态,还没有刷到主机
		WARCRAFT3_WAIT1_STATUS    = 2,		// 已经刷到主机,还没有加入
		WARCRAFT3_JOIN_STATUS     = 3,		// 已经请求主机状态
		WARCRAFT3_CREATE_STATUS   = 4,		// 已经创建主机
		WARCRAFT3_JOIN1_STATUS    = 5,		// 用户已经加入游戏
		WARCRAFT3_GAMEING_STATUS  = 6,		// 已经进入游戏状态
		WARCRAFT3_GAME_CLOSED     = 7,		// 游戏已经结束了
		WARCRAFT3_GAME_OVER       = 8,
	};

	extern bool             open_map_flag;
	extern bool             enable_auto_enter;
	extern WARCRAFT3_STATUS status;

	void initialize(HMODULE gamedll);
}}
