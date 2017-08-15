#include <base/path/helper.h>
#include <base/util/foreach.h>
#include <deque>

namespace base { namespace path {

	fs::path normalize(const fs::path& p)
	{
		fs::path result = p.root_path();
		std::deque<std::wstring> stack;
		foreach (auto e, p.relative_path()) {
			if (e == L".." && !stack.empty() && stack.back() != L"..") {
				stack.pop_back();
			}
			else if (e != L".") {
				stack.push_back(e.wstring());
			}
		}
		foreach (auto e, stack) {
			result /= e;
		}
		std::wstring ret = result.wstring();
		std::wstring tmp;
		tmp.resize(ret.size());
		std::transform(ret.begin(), ret.end(), tmp.begin(), [](unsigned char c) { return (unsigned char)tolower(c); });
		return tmp;
	}

	bool equal(fs::path const& lhs, fs::path const& rhs)
	{
		fs::path lpath = normalize(lhs);
		fs::path rpath = normalize(rhs);
		const fs::path::value_type* l(lpath.c_str());
		const fs::path::value_type* r(rpath.c_str());
		while ((tolower(*l) == tolower(*r) || (*l == L'\\' && *r == L'/') || (*l == L'/' && *r == L'\\')) && *l) 
		{ 
			++l; ++r; 
		}
		return *l == *r;
	}
}}
