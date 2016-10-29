#include <base/path/helper.h>

namespace base { namespace path {

	bool equal(fs::path const& lhs, fs::path const& rhs)
	{
		fs::path lpath(lhs);
		fs::path rpath(rhs);
		const fs::path::value_type* l(lpath.normalize().c_str());
		const fs::path::value_type* r(rpath.normalize().c_str());
		while ((tolower(*l) == tolower(*r) || (*l == L'\\' && *r == L'/') || (*l == L'/' && *r == L'\\')) && *l) 
		{ 
			++l; ++r; 
		}
		return *l == *r;
	}
}}
