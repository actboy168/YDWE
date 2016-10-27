#include "StdAfx.h"
#include "../zip/zip_reader.h"
#include <base/path/helper.h>

namespace DuiLib
{
	CUIBuffer::CUIBuffer()
		: ptrData()
		, nSize(0)
	{ }

	CUIBuffer::CUIBuffer(size_t size)
		: ptrData(new unsigned char[size])
		, nSize(size)
	{ }

	CUIBuffer::CUIBuffer(CUIBuffer&& that)
		: ptrData(std::move(that.ptrData))
		, nSize(that.nSize)
	{ }

	void CUIBuffer::reset(size_t size)
	{ 
		if (size == 0) 
			ptrData.reset();
		else
			ptrData.reset(new unsigned char[size]);
		nSize = size;
	}

	CUIBuffer::operator bool() const
	{
		return (nSize > 0) && ptrData;
	}

	CUIBuffer& CUIBuffer::operator=(CUIBuffer&& that)
	{
		if (this != &that)
		{
			ptrData = std::move(that.ptrData);
			nSize = that.nSize;
		}
		return (*this);
	}

	CUIBuffer CUIFile::LoadZip(fs::path const& name)
	{
		fs::path zip_file = CPaintManagerUI::GetResourceZip();

		zip::reader r;
		if (r.Open(zip_file)) 
		{
			for (auto it = r.begin(); it != r.end(); ++it) 
			{
				zip::reader::entry& current_entry_info = *it;
				if (current_entry_info.is_vaild()
					&& (!current_entry_info.is_directory())
					&& (base::path::equal(current_entry_info.file_path(), name)))
				{
					CUIBuffer buf;
					buf.reset((size_t)current_entry_info.original_size());
					if (current_entry_info.extract_to_buffer(buf.ptrData.get(), (size_t)current_entry_info.original_size()))
					{
						return std::move(buf);
					}
				}
			}
		}

		return std::move(CUIBuffer());
	}

	CUIBuffer CUIFile::LoadFile(fs::path const& name)
	{
		CUIBuffer buf;

		HANDLE hFile = ::CreateFileW(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) 
			return std::move(buf);

		DWORD dwSize = ::GetFileSize(hFile, NULL);
		if (dwSize == 0) 
			return std::move(buf);

		buf.reset(dwSize);
		DWORD dwRead = 0;
		BOOL r = ::ReadFile(hFile, buf.ptrData.get(), dwSize, &dwRead, NULL);
		::CloseHandle(hFile);

		if (!r || (dwRead != dwSize))
		{
			buf.reset();
		}

		return std::move(buf);
	}
}
