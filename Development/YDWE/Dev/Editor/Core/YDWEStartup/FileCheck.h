#ifndef FILE_CHECK_H_INCLUDED
#define FILE_CHECK_H_INCLUDED

#include <string>
#include <boost/utility.hpp>
#include <boost/system/system_error.hpp>
#include <boost/filesystem.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/cstdint.hpp>
#include <windows.h>

namespace NYDWE
{

std::string CalculateFileRawMd5(const boost::filesystem::path &filePath, boost::system::error_code *pErrorCode = NULL);
std::string CalculateMemoryRawMd5(const void *pBegin, const void *pEnd);
bool FileContentEqual(const boost::filesystem::path &fileFirst, const boost::filesystem::path &fileSecond, boost::system::error_code *pErrorCode = NULL);

class CFileMemoryMapper : boost::noncopyable
{
public:
	static const int READ = 1;
	static const int WRITE = 2;

public:
	CFileMemoryMapper(const boost::filesystem::path &filePath, int access);
	~CFileMemoryMapper();

public:
	boost::uint8_t *begin();
	boost::uint8_t *end();
	size_t size();

private:
	HANDLE fileHandle_;
	HANDLE mappingHandle_;
	LPVOID viewAddress_;
};

}

#endif // FILE_CHECK_H_INCLUDED
