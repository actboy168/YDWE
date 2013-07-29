#include <ydwe/path/filesystem_helper.h>

namespace ydwe { namespace path {

	bool equal(boost::filesystem::path const& lhs, boost::filesystem::path const& rhs)
	{
		boost::filesystem::path lpath(lhs);
		boost::filesystem::path rpath(rhs);
		const boost::filesystem::path::value_type* l(lpath.normalize().c_str());
		const boost::filesystem::path::value_type* r(rpath.normalize().c_str());
		while ((tolower(*l) == tolower(*r) || (*l == L'\\' && *r == L'/') || (*l == L'/' && *r == L'\\')) && *l) 
		{ 
			++l; ++r; 
		}
		return *l == *r;
	}
}}
