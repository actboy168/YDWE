#include <base/path/helper.h>
#include <base/path/get_path.h>
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
		std::transform(ret.begin(), ret.end(), tmp.begin(), towlower);
		return tmp;
	}

	fs::path uncomplete(const fs::path& path, const fs::path& base, std::error_code& ec)
	{
		fs::path npath = normalize(path);
		fs::path nbase = normalize(base);
		if (npath == nbase)
			return L"./";
		fs::path from_path, from_base, output;
		fs::path::iterator path_it = npath.begin(), path_end = npath.end();
		fs::path::iterator base_it = nbase.begin(), base_end = nbase.end();

		if ((path_it == path_end) || (base_it == base_end))
		{
			ec = std::make_error_code(std::errc::not_a_directory);
			return npath;
		}

#ifdef WIN32
		if (*path_it != *base_it)
			return npath;
		++path_it, ++base_it;
#endif

		while (true) {
			if ((path_it == path_end) || (base_it == base_end) || (*path_it != *base_it)) {
				for (; base_it != base_end; ++base_it) {
					if (*base_it == L".")
						continue;
					else if (*base_it == L"/")
						continue;

					output /= L"../";
				}
				fs::path::iterator path_it_start = path_it;
				for (; path_it != path_end; ++path_it) {

					if (path_it != path_it_start)
						output /= L"/";

					if (*path_it == L".")
						continue;
					if (*path_it == L"/")
						continue;

					output /= *path_it;
				}

				break;
			}
			from_path /= fs::path(*path_it);
			from_base /= fs::path(*base_it);
			++path_it, ++base_it;
		}

		return output;
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
