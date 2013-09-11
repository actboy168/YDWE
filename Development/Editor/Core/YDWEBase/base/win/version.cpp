#include <base/win/version.h>
#include <Windows.h>

_BASE_BEGIN
namespace win {

	Version version()
	{
		Version version_ = VERSION_PRE_XP;
		OSVERSIONINFOW osvi = { sizeof OSVERSIONINFOW };
		::GetVersionExW(&osvi);
		if ((osvi.dwMajorVersion == 5) && (osvi.dwMinorVersion > 0)) 
		{
			version_ = (osvi.dwMinorVersion == 1) ? VERSION_XP : VERSION_SERVER_2003;
		} 
		else if (osvi.dwMajorVersion == 6)
		{
			switch (osvi.dwMinorVersion) 
			{
			case 0:
				version_ = VERSION_VISTA;
				break;
			case 1:
				version_ = VERSION_WIN7;
				break;
			default:
				version_ = VERSION_WIN8;
				break;
			}
		} 
		else if (osvi.dwMajorVersion > 6) 
		{
			version_ = VERSION_WIN_LAST;
		}

		return version_;
	}
}
_BASE_END
