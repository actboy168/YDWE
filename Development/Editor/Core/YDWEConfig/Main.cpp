#include <boost/exception/all.hpp>
#include <windows.h>
#include "MainWindow.h"
#include <base/com/guard.h>
#include <base/util/unicode.h>


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPWSTR lpCmdLine, int nCmdShow)
{
	if (lpCmdLine && lpCmdLine == std::wstring(L"-launchwar3"))
	{
		bool launch_warcraft3();
		launch_warcraft3();
		return 1;
	}

	try
	{
		ydwe::com::guard com;

		DuiLib::CPaintManagerUI::SetInstance(hInstance);

		CMainWindow* pFrame = new CMainWindow();
		pFrame->Create(NULL, _T("YDWEÅäÖÃ³ÌÐò"), UI_WNDSTYLE_DIALOG, 0);
		pFrame->CenterWindow();
		pFrame->ShowWindow(true);
		DuiLib::CPaintManagerUI::MessageLoop();
	}
	catch (std::exception const& e)
	{
		::MessageBoxW(NULL, ydwe::util::u2w(e.what(), ydwe::util::conv_method::replace | '?').c_str(), L"ERROR", MB_ICONERROR | MB_OK);
	}
	catch (...)
	{
		::MessageBoxA(NULL, boost::current_exception_diagnostic_information().c_str(), "ERROR", MB_ICONERROR | MB_OK);
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
