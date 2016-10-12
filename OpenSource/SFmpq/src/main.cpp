#define __STORMLIB_SELF__
#include <stormlib.h>

#include <string>
#include <vector>

bool AnsiToWide(const std::string& str, std::wstring& wideString)
{
	if (str.empty()) {
		return false;
	}
	int nLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	if (nLen <= 0) {
		wideString = L"";
		return false;
	}
	std::vector<wchar_t> resultstring(nLen);
	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &resultstring[0], nLen);
	wideString = std::wstring(&resultstring[0]);
	return true;
}

std::wstring AnsiToWide(const std::string& str)
{
	std::wstring wideStr;
	if (str.empty()) {
		return wideStr;
	}
	if (!AnsiToWide(str, wideStr)) {
		wideStr = L"";
	}
	return wideStr;
}

BOOL WINAPI MpqCompactArchive(HANDLE hMpq)
{
	return SFileCompactArchive(hMpq, 0, 0);
}

BOOL WINAPI MpqDeleteFile(HANDLE hMpq, const char* szFileName)
{
	if (strcmp(szFileName, "(attributes)") == 0) {
		return TRUE;
	}
	return SFileRemoveFile(hMpq, szFileName, 0);
}

BOOL WINAPI MpqCloseUpdatedArchive(HANDLE hMpq, DWORD dwUnknown)
{
	return SFileCloseArchive(hMpq);
}

BOOL WINAPI MpqAddFileToArchiveEx(HANDLE hMpq, LPCSTR szFileName, LPCSTR szArchivedName, DWORD dwFlags, DWORD dwCompressionType, DWORD dwCompressLevel)
{
	return SFileAddFileEx(hMpq, AnsiToWide(szFileName).c_str(), szArchivedName, dwFlags, dwCompressionType, dwCompressionType);
}

HANDLE WINAPI MpqOpenArchiveForUpdate(LPCSTR szFileName, DWORD dwFlags, DWORD dwMaximumFilesInArchive)
{
	HANDLE hMpq = 0;
	if (SFileCreateArchive(AnsiToWide(szFileName).c_str(), dwFlags, dwMaximumFilesInArchive, &hMpq))
	{
		return hMpq;
	}
	if (SFileOpenArchive(AnsiToWide(szFileName).c_str(), 0, dwFlags, &hMpq))
	{
		SFileSetMaxFileCount(hMpq, dwMaximumFilesInArchive);
		return hMpq;
	}
	return 0;
}
