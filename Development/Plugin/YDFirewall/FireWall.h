#pragma once

#include <winsock2.h>

BOOL FireWallAddApplication(LPCWSTR lpszProcessImageFileName, LPCWSTR lpszRegisterName);

// TODO: 考虑用继承来封装一下，如下：

/*
struct CWindowsFirewall
{
	virtual BOOL addApplication(LPCWSTR imageFireName, LPCWSTR registerName) = 0;
	// ...
};

class CXXXFirewall : public CWindowsFirewall
{
	// 实现函数
};

*/