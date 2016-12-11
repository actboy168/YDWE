#include <Windows.h>
#include <string>
#include <memory>
#include <base/hook/iat.h>
#include <base/hook/fp_call.h>
#include <base/util/unicode.h>
#include <base/win/font/utility.h>

class FontManager
{
public:
	FontManager(const char* name, size_t size);
	~FontManager();
	void postWindow(HWND hWnd);

private:
	HFONT font_;
};

std::unique_ptr<FontManager> g_fontptr;

namespace real
{
	uintptr_t CreateWindowExA = 0;
}

namespace fake
{
	HWND WINAPI CreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
	{
		HWND  hWnd = base::std_call<HWND>(real::CreateWindowExA, dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
		if (g_fontptr) g_fontptr->postWindow(hWnd);
		return hWnd;
	}
}

FontManager::FontManager(const char* name, size_t size)
: font_(NULL)
{
	font_ = ::CreateFontW(
		base::font::size_to_height(size), //字体的逻辑高度
		0,                                //逻辑平均字符宽度
		0,                                //与水平线的角度
		0,                                //基线方位角度
		FW_DONTCARE,                      //字形：常规
		FALSE,                            //字形：斜体
		FALSE,                            //字形：下划线
		FALSE,                            //字形：粗体
		DEFAULT_CHARSET,                  //字符集
		OUT_DEFAULT_PRECIS,               //输出精度
		CLIP_DEFAULT_PRECIS,              //剪截精度
		DEFAULT_QUALITY,                  //输出品质
		DEFAULT_PITCH | FF_DONTCARE,      //倾斜度
		base::u2w(name, base::conv_method::replace | '?').c_str() //字体
		);

	if (font_ != NULL)
	{
		real::CreateWindowExA = base::hook::iat(
			::GetModuleHandle(NULL),
			"user32.dll",
			"CreateWindowExA",
			(uintptr_t)fake::CreateWindowExA);
	}
}

FontManager::~FontManager()
{
	if (font_)
	{
		::DeleteObject(font_);
		base::hook::iat(::GetModuleHandle(NULL),
			"user32.dll",
			"CreateWindowExA",
			(uintptr_t)fake::CreateWindowExA);
	}
}

void FontManager::postWindow(HWND hWnd)
{
	if (font_) ::PostMessage(hWnd, WM_SETFONT, (WPARAM)(HFONT)(font_), (LPARAM)(BOOL)(0));
}

bool SetFontByName(const char* name, size_t size)
{
	g_fontptr.reset(new FontManager(name, size));
	return true;
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID pReserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		::DisableThreadLibraryCalls(module);
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		g_fontptr.reset();
	}
	return TRUE;
}

const char *PluginName()
{
	return "YDFont";
}
