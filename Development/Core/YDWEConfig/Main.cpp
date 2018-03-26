#include <windows.h>
#include "MainWindow.h"
#include <base/com/guard.h>
#include <base/util/unicode.h>
#include <base/i18n-2/gettext.h>
#include <base/path/ydwe.h>

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR lpCmdLine, int nCmdShow)
{
	try
	{
		base::com::guard com;

		base::i18n::v2::initialize(base::path::ydwe(true) / "share" / "locale");
		base::i18n::v2::set_domain(L"config");

		DuiLib::CPaintManagerUI::SetInstance(hInstance);
		DuiLib::CPaintManagerUI::SetLanguage(base::i18n::v2::get_language());
		
		CMainWindow* pFrame = new CMainWindow();
		pFrame->Create(NULL, L"YDWEÅäÖÃ³ÌÐò", UI_WNDSTYLE_DIALOG, 0);
		pFrame->CenterWindow();
		pFrame->ShowWindow(true);
		DuiLib::CPaintManagerUI::MessageLoop();
	}
	catch (std::exception const& e)
	{
		::MessageBoxW(NULL, base::u2w(e.what(), base::conv_method::replace | '?').c_str(), L"ERROR", MB_ICONERROR | MB_OK);
	}
	catch (...)
	{
		MessageBoxW(NULL, L"Unknown error", L"ERROR", MB_OK | MB_ICONERROR);
	}

    return 0;
}

#if defined _M_IX86
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_AMD64
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
