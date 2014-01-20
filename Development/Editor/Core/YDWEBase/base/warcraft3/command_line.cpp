#include <base/warcraft3/command_line.h>
#include <Windows.h>

namespace base { namespace warcraft3 {
	void parse_command_line(std::function<void(std::wstring const&, std::wstring const&)> proc)
	{
		int argc;
		wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);

		if (argv)
		{
			std::wstring key;
			std::wstring val;
			for (int i = 1; i < argc; ++i)
			{
				if (argv[i][0] == '-')
				{
					if (!key.empty())
					{
						proc(key, val);
						val.clear();
					}
					key = argv[i] + 1;
				}
				else
				{
					if (!val.empty())
					{
						val.push_back(' ');
					}
					val.append(argv[i]);
				}
			}

			if (!key.empty())
			{
				proc(key, val);
			}

			::LocalFree(argv);
		}
	}
}}
