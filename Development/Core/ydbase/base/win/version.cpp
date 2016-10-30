#include <base/win/version.h>
#include <Windows.h>
#include <base/win/file_version.h>

namespace base { namespace win {

	version_number get_version_number()
	{
		version_number vn;

		OSVERSIONINFOW osvi = { sizeof OSVERSIONINFOW };
#pragma warning(suppress:4996)
		::GetVersionExW(&osvi);
		
		vn.major = osvi.dwMajorVersion;
		vn.minor = osvi.dwMinorVersion;
		vn.build = osvi.dwBuildNumber;

		if ((vn.major > 6) || (vn.major == 6 && vn.minor >= 2))
		{
			// see
			//   http://msdn.microsoft.com/en-us/library/windows/desktop/ms724451(v=vs.85).aspx
			//   http://msdn.microsoft.com/en-us/library/windows/desktop/ms724429(v=vs.85).aspx

			simple_file_version sfv(::GetModuleHandleW(L"kernel32.dll"), L"ProductVersion", L'.');

			vn.major = sfv.major;
			vn.minor = sfv.minor;
			vn.build = sfv.revision;
		}

		return vn;
	}

	version get_version()
	{
		version_number vn = get_version_number();
		version        v  = VERSION_PRE_XP;

		if ((vn.major == 5) && (vn.minor > 0)) 
		{
			v = (vn.minor == 1) ? VERSION_XP : VERSION_SERVER_2003;
		} 
		else if (vn.major == 6)
		{
			switch (vn.minor) 
			{
			case 0:
				v = VERSION_VISTA;
				break;
			case 1:
				v = VERSION_WIN7;
				break;
			case 2:
				v = VERSION_WIN8;
				break;
			case 3:
				v = VERSION_WIN8_1;
				break;
			case 4:
				v = VERSION_WIN10;
				break;
			default:
				v = VERSION_WIN_LAST;
				break;
			}
		} 
		else if (vn.major > 6) 
		{
			v = VERSION_WIN_LAST;
		}

		return v;
	}
}}
