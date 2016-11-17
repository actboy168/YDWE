#include <base/path/helper.h>
#include <deque>

namespace base { namespace path {

	fs::path normalize(const fs::path& p)
	{
		fs::path result = p.root_path();
		std::deque<std::string> stack;
		for (auto e : p.relative_path()) {
			if (e == ".." && !stack.empty() && stack.back() != "..") {
				stack.pop_back();
			}
			else if (e != ".") {
				stack.push_back(e.string());
			}
		}
		for (auto e : stack) {
			result /= e;
		}
		std::string ret = result.string();
		std::string tmp;
		tmp.resize(ret.size());
		std::transform(ret.begin(), ret.end(), tmp.begin(), tolower);
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
