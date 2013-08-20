#ifndef FILE_CHECK_H_INCLUDED
#define FILE_CHECK_H_INCLUDED

#include <boost/system/system_error.hpp>
#include <boost/filesystem.hpp>

namespace NYDWE
{
bool FileContentEqual(const boost::filesystem::path &fileFirst, const boost::filesystem::path &fileSecond, boost::system::error_code *pErrorCode = NULL);
}

#endif // FILE_CHECK_H_INCLUDED
