#include <base/warcraft3/command_line.h>
#include <base/util/foreach.h>
#include <base/util/list_of.h>
#include <Windows.h>
#include <set>

namespace base { namespace warcraft3 {

	std::wstring command_line::parse(std::function<void(std::wstring const&, std::wstring const&)> proc)
	{
		std::wstring result;

		int argc;
		wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);
		if (argv)
		{
			result = argv[0];

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

		return std::move(result);
	}

	command_line::command_line()
		: app_()
	{
		app_ = parse([this](std::wstring const& key, std::wstring const& val) {
			add(key, val);
		});
	}

	static std::wstring get_str(const std::pair<std::wstring, std::wstring>& item)
	{
		std::wstring r = L" -" + item.first;
		if (!item.second.empty())
		{
			r += L" \"" + item.second + L"\"";
		}
		return r;
	}

	static std::set<std::wstring> knowns =
#if _MSC_VER < 1800
		base::list_of(L"window")(L"opengl")(L"loadfile")
#else
	{
		L"window", L"opengl", L"loadfile"
	}
#endif
	;

	std::wstring command_line::str() const
	{
		std::wstring result = L"\"" + app_ + L"\"";
		std::wstring unknown = L"";

		foreach(auto item, *this)
		{
			if (knowns.find(item.first) != knowns.end())
			{
				result += get_str(item);
			}
			else
			{
				unknown += get_str(item);
			}
		}
		return result + unknown;
	}

	bool command_line::app(const std::wstring& v)
	{
		app_ = v;
		return true;
	}

	bool command_line::has(const std::wstring& key) const
	{
		return this->find(key) != this->end();
	}

	bool command_line::del(const std::wstring& key)
	{
		this->erase(key);
		return true;
	}

	bool command_line::add(const std::wstring& key)
	{
		return add(key, std::wstring());
	}

	bool command_line::add(const std::wstring& key, const std::wstring& val)
	{
		return this->insert(std::make_pair(key, val)).second;
	}
}}
