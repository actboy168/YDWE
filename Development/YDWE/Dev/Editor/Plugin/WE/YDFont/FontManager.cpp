
#include "FontManager.hpp"
#include <ydwe/hook/iat.h>

std::unique_ptr<FontManager> g_fontptr;

namespace font
{
	std::string get_name(std::wstring const& szFilePath);
}

namespace 
{
	typedef HWND (WINAPI* fnCreateWindowExA)(
		DWORD dwExStyle,
		LPCSTR lpClassName,
		LPCSTR lpWindowName,
		DWORD dwStyle,
		int x,
		int y,
		int nWidth,
		int nHeight,
		HWND hWndParent,
		HMENU hMenu,
		HINSTANCE hInstance,
		LPVOID lpParam
		);
	fnCreateWindowExA CreateWindowExANextHook;


	HWND WINAPI CreateWindowExAHook(
		DWORD dwExStyle,
		LPCSTR lpClassName,
		LPCSTR lpWindowName,
		DWORD dwStyle,
		int x,
		int y,
		int nWidth,
		int nHeight,
		HWND hWndParent,
		HMENU hMenu,
		HINSTANCE hInstance,
		LPVOID lpParam
		)
	{
		HWND  hWnd = CreateWindowExANextHook(dwExStyle, lpClassName, lpWindowName, dwStyle, 
			x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

		// "BUTTON"
		// "STATIC"
		// "SCROLLBAR"
		// "COMBOBOX"
		// "LISTBOX"
		// "EDIT"
		// "ToolbarWindow32"
		// "tooltips_class32"
		// "msctls_updown32"
		// "msctls_progress32"
		// "SysTreeView32"
		// "SysListView32"
		// "SysTabControl32"

		if (g_fontptr) g_fontptr->postWindow(hWnd);
		return hWnd;
	}
}

class Config
{
public:
	Config(fs::path path)
		: path_(path)
	{ }
	
	int getInt(const wchar_t* section, const wchar_t* key, int def = 0)
	{
		return ::GetPrivateProfileIntW(section, key, def, path_.wstring().c_str());
	}

	std::wstring getStr(const wchar_t* section, const wchar_t* key, const wchar_t* def = L"")
	{
		std::unique_ptr<wchar_t[]> buffer;
		size_t nsize = 64;
		size_t retsize = 0;
		do 
		{
			nsize <<= 1;
			buffer.reset(new wchar_t[nsize]);
			retsize = ::GetPrivateProfileStringW(section, key, def, buffer.get(), nsize, path_.wstring().c_str());
		} while (nsize-1 == retsize);

		return std::move(std::wstring(buffer.get(), retsize));
	}
private:
	fs::path path_;
};

FontManager::FontManager(fs::path&& module_path)
	: font_(NULL)
{
	Config cfg(module_path / "font.ini");
	
	if (0 == cfg.getInt(L"Font", L"Enable"))
	{
		return ;
	}

	font_path_ = module_path / cfg.getStr(L"Font", L"Font");
	
	if (0 != ::AddFontResource(font_path_.string().c_str()))
	{
		std::string szFontName = font::get_name(font_path_.wstring());

		font_ = ::CreateFontA(
			cfg.getInt(L"Font", L"Height"),   //字体的逻辑高度
			cfg.getInt(L"Font", L"Width"),    //逻辑平均字符宽度
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
			DEFAULT_PITCH|FF_DONTCARE,        //倾斜度
			szFontName.c_str()              //字体
			);

		if (font_ != NULL)
		{
			CreateWindowExANextHook = (fnCreateWindowExA)ydwe::hook::iat(
				::GetModuleHandle(NULL),
				"user32.dll",
				"CreateWindowExA",
				(uintptr_t)CreateWindowExAHook);
		}
	}
}

FontManager::~FontManager()
{
	::RemoveFontResource(font_path_.string().c_str());

	if (font_)
	{
		::DeleteObject(font_);
		ydwe::hook::iat(::GetModuleHandle(NULL),
			"user32.dll",
			"CreateWindowExA",
			(uintptr_t)CreateWindowExANextHook);
	}
}

void FontManager::postWindow(HWND hWnd)
{
	if (font_) ::PostMessage(hWnd, WM_SETFONT, (WPARAM)(HFONT)(font_), (LPARAM)(BOOL)(0));
}
