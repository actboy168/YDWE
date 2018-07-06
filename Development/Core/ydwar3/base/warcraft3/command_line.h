#pragma once

#include <base/warcraft3/config.h>
#include <functional>
#include <map>
#include <string>

namespace base { namespace warcraft3 {

#pragma warning (push)
#pragma warning (disable:4251)
	class _WAR3_API command_line
		: public std::map<std::wstring, std::wstring>
	{
	public:
		command_line(); 
		std::wstring str() const;
		bool         app(const std::wstring& v);
		bool         has(const std::wstring& key) const;
		bool         del(const std::wstring& key);
		bool         add(const std::wstring& key);
		bool         add(const std::wstring& key, const std::wstring& val);

	private:
		std::wstring app_;

	public:
		static std::wstring parse(std::function<void(std::wstring const&, std::wstring const&)> proc);
	};
#pragma warning (pop)

}}

