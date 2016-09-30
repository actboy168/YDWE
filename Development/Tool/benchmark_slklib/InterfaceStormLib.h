#pragma once

#include <deque>
#include <vector>
#define __STORMLIB_SELF__
#define __STORMLIB_NO_AUTO_LINKING__
#include <StormLib.h>
#include <slk/InterfaceStorm.hpp>

class MPQ
{
public:
	MPQ(HANDLE handle, bool need_close = true)
		: handle_(handle)
		, need_close_(need_close)
	{ }

	void close() const
	{
		if (need_close_) 
		{
			SFileCloseArchive(handle_);
		}
	}

	HANDLE handle() const
	{
		return handle_;
	}

private:
	HANDLE handle_;
	bool need_close_;
};

class InterfaceStormLib : public slk::InterfaceStorm
{
public:
	~InterfaceStormLib()
	{
		foreach(MPQ const& mpq, archivelist_)
		{
			mpq.close();
		}
	}

	std::string load(std::string const& szFileName, error_code& ec)
	{
		HANDLE hFile;
		if (!open_file(szFileName, &hFile))
		{
			ec = ERROR_FILE_NOT_FOUND;
			return std::move(std::string());
		}

		DWORD hSize = SFileGetFileSize(hFile, NULL);
		std::vector<char> buf(hSize, 0);
		if (hSize != 0L)
		{
			SFileReadFile(hFile, &*buf.begin(), hSize, NULL, NULL);
		}
		SFileCloseFile(hFile);
		ec = ERROR_SUCCESS;
		return std::move(std::string(buf.begin(), buf.end()));
	}

	std::string load(std::string const& szFileName)
	{
		error_code ec = ERROR_SUCCESS;
		return std::move(load(szFileName, ec));
	}

	bool has(std::string const& szFileName)
	{
		foreach(MPQ const& mpq, archivelist_)
		{
			if (SFileHasFile(mpq.handle(), szFileName.c_str()))
			{
				return true;
			}
		}
		return false;
	}

	bool open_archive(std::string const& szArchivename, bool read_only = true)
	{
		HANDLE hArchive;
		bool b = SFileOpenArchive(szArchivename.c_str(), 0, read_only? MPQ_OPEN_READ_ONLY: 0, &hArchive);

		if (b)
		{
			archivelist_.push_front(hArchive);
		}

		return b;
	}

	bool attach_archive(HANDLE hArchive)
	{
		archivelist_.push_front(MPQ(hArchive, false));
		return true;
	}

private:
	bool open_file(std::string const& szFileName, HANDLE* phFile)
	{
		foreach(MPQ const& mpq, archivelist_)
		{
			if (SFileOpenFileEx(mpq.handle(), szFileName.c_str(), SFILE_OPEN_FROM_MPQ, phFile))
			{
				return true;
			}
		}
		return false;
	}

	std::deque<MPQ> archivelist_;
};
