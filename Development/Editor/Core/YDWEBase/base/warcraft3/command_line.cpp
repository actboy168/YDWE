#include <base/warcraft3/command_line.h>
#include <base/util/foreach.h>
#include <Windows.h>

namespace base { namespace warcraft3 {

	void command_line::parse(std::function<void(std::wstring const&, std::wstring const&)> proc)
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

	command_line::command_line()
	{
		parse([this](std::wstring const& key, std::wstring const& val){
			insert(std::make_pair(key, val));
		});
	}

	std::wstring command_line::str() const
	{
		std::wstring result;
		foreach(auto it, *this)
		{
			result += L" -";
			result += it.first;
			if (!it.second.empty())
			{
				result += L" \"";
				result += it.second;
				result += L" \"";
			}
		}

		return std::move(result);
	}
}}
