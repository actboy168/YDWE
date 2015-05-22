#include <base/warcraft3/command_line.h>
#include <base/util/foreach.h>
#include <Windows.h>

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
		app_ = parse([this](std::wstring const& key, std::wstring const& val){
			add(key, val);
		});
	}

	std::wstring command_line::str() const
	{
		std::wstring result;

		result += L"\"";
		result += app_;
		result += L"\"";

		if (has(L"window")) { result += L" -window"; }
		if (has(L"opengl")) { result += L" -opengl"; }

		foreach(auto it, *this)
		{
			if (it.first != L"window" &&it.first != L"opengl")
			{
				result += L" -";
				result += it.first;
				if (!it.second.empty())
				{
					result += L" \"";
					result += it.second;
					result += L"\"";
				}
			}
		}

		return std::move(result);
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
