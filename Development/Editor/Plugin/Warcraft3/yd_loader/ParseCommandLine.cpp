#include "ParseCommandLine.h"
#include <Windows.h>

void ParseCommandLine(std::function<void(std::wstring const&, std::wstring const&)> Proc)
{
	int nArgs;
	wchar_t** szArglist = ::CommandLineToArgvW(::GetCommandLineW(), &nArgs);

	if (szArglist)
	{
		std::wstring key;
		std::wstring val;
		for (int i = 1; i < nArgs; ++i)
		{
			if (szArglist[i][0] == '-')
			{
				if (!key.empty())
				{
					Proc(key, val);
					val.clear();
				}
				key = szArglist[i]+1;
			}
			else
			{
				if (!val.empty())
				{
					val.push_back(' ');
				}
				val.append(szArglist[i]);
			}
		}

		if (!key.empty())
		{
			Proc(key, val);
		}

		::LocalFree(szArglist);
	}
}
