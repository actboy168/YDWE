#include "FileCheck.h"
#include <ydwe/file/memory_mapped_file.h>

namespace NYDWE
{

bool FileContentEqual(const boost::filesystem::path &fileFirst, const boost::filesystem::path &fileSecond, boost::system::error_code *pErrorCode)
{
	try
	{
		if (pErrorCode)
			pErrorCode->assign(boost::system::errc::success, boost::system::generic_category());

		ydwe::file::memory_mapped_file mapperFirst(fileFirst.c_str());
		ydwe::file::memory_mapped_file mapperSecond(fileSecond.c_str());

		size_t size;
		return (size = mapperFirst.size()) == mapperSecond.size() && memcmp(mapperFirst.memory(), mapperSecond.memory(), size) == 0;
	}
	catch (boost::system::system_error &e)
	{
		if (pErrorCode)
			*pErrorCode = e.code();
		return false;
	}
}

}
