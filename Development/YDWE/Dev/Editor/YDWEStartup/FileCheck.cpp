#include <shlwapi.h>
#include <windows.h>
#include <boost/exception/all.hpp>
#include <boost/scope_exit.hpp>
#include "md5.h"
#include "FileCheck.h"

namespace NYDWE
{

std::string CalculateFileRawMd5(const boost::filesystem::path &filePath, boost::system::error_code *pErrorCode)
{
	try
	{
		if (pErrorCode)
			pErrorCode->assign(boost::system::errc::success, boost::system::system_category());

		CFileMemoryMapper mapper(filePath, CFileMemoryMapper::READ);
		return std::move(CalculateMemoryRawMd5(mapper.begin(), mapper.end()));
	}
	catch (boost::system::system_error &e)
	{
		if (pErrorCode)
			*pErrorCode = e.code();
		return std::move(std::string());
	}
}

std::string CalculateMemoryRawMd5(const void *pBegin, const void *pEnd)
{
	MD5 md;
	md.update((boost::uint8_t *)pBegin, (boost::uint8_t *)pEnd - (boost::uint8_t *)pBegin);
	md.finalize();
	return std::move(md.hexdigest());
}

bool FileContentEqual(const boost::filesystem::path &fileFirst, const boost::filesystem::path &fileSecond, boost::system::error_code *pErrorCode)
{
	try
	{
		if (pErrorCode)
			pErrorCode->assign(boost::system::errc::success, boost::system::generic_category());

		CFileMemoryMapper mapperFirst(fileFirst, CFileMemoryMapper::READ),
			mapperSecond(fileSecond, CFileMemoryMapper::READ);

		size_t size;
		return (size = mapperFirst.size()) == mapperSecond.size() && memcmp(mapperFirst.begin(), mapperSecond.begin(), size) == 0;
	}
	catch (boost::system::system_error &e)
	{
		if (pErrorCode)
			*pErrorCode = e.code();
		return false;
	}
}

CFileMemoryMapper::CFileMemoryMapper(const boost::filesystem::path &filePath, int access)
	: fileHandle_(INVALID_HANDLE_VALUE), mappingHandle_(NULL), viewAddress_(NULL)
{
	BOOL success = FALSE;
	BOOST_SCOPE_EXIT( (&success)(&fileHandle_)(&mappingHandle_) )
	{
		if (!success)
		{
			if (fileHandle_ != INVALID_HANDLE_VALUE)
				CloseHandle(fileHandle_);
			if (mappingHandle_ != NULL)
				CloseHandle(mappingHandle_);
		}
	} BOOST_SCOPE_EXIT_END

	DWORD desiredAccess = (access & READ ? GENERIC_READ : 0) | (access & WRITE ? GENERIC_WRITE : 0);
	DWORD creationDisposition = access & WRITE ? CREATE_ALWAYS : OPEN_ALWAYS;
	fileHandle_ = CreateFileW(filePath.c_str(), desiredAccess, FILE_SHARE_READ, NULL, creationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileHandle_ == INVALID_HANDLE_VALUE)
		BOOST_THROW_EXCEPTION(boost::system::system_error(GetLastError(), boost::system::system_category()));

	// Test file of 0 length
	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(fileHandle_, &fileSize))
		BOOST_THROW_EXCEPTION(boost::system::system_error(GetLastError(), boost::system::system_category()));

	if (!fileSize.QuadPart)
		BOOST_THROW_EXCEPTION(boost::system::system_error(INVALID_FILE_SIZE, boost::system::system_category()));
	
	DWORD pageProtection = access & WRITE ? PAGE_READWRITE : PAGE_READONLY;
	mappingHandle_ = CreateFileMappingW(fileHandle_, NULL, pageProtection, 0, 0, NULL);
	if (mappingHandle_ == NULL)
		BOOST_THROW_EXCEPTION(boost::system::system_error(GetLastError(), boost::system::system_category()));

	DWORD mapAccess = access & WRITE ? FILE_MAP_WRITE : FILE_MAP_READ;
	viewAddress_ = MapViewOfFile(mappingHandle_, mapAccess, 0, 0, 0);
	if (viewAddress_ == NULL)
		BOOST_THROW_EXCEPTION(boost::system::system_error(GetLastError(), boost::system::system_category()));

	success = TRUE;
}

CFileMemoryMapper::~CFileMemoryMapper()
{
	UnmapViewOfFile(viewAddress_);
	CloseHandle(mappingHandle_);
	CloseHandle(fileHandle_);
}

boost::uint8_t *CFileMemoryMapper::begin()
{
	return (boost::uint8_t *)viewAddress_;
}

boost::uint8_t *CFileMemoryMapper::end()
{
	return begin() + size();
}

size_t CFileMemoryMapper::size()
{
	LARGE_INTEGER fileSize;
	if (!GetFileSizeEx(fileHandle_, &fileSize))
		BOOST_THROW_EXCEPTION(boost::system::system_error(GetLastError(), boost::system::system_category()));

#ifdef _WIN64
	return fileSize.QuadPart;
#else
	return fileSize.HighPart ? 0xFFFFFFFFL : fileSize.LowPart;
#endif
}

}
