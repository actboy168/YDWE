#include "zip_reader.h"
#include "zip_internal.h"

#include <fstream>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>
#include <ydwe/exception/windows_exception.h>
#include <minizip/unzip.h>
#include <minizip/iowin32.h>

namespace zip {

	namespace detail
	{
		void zip_reader_iterator_construct(ZipReader::iterator& it, const unzFile* zf_ptr, std::error_code* ec)
		{
			if (unzGoToFirstFile(*zf_ptr) != UNZ_OK)
			{
				it.impl_.reset();

				ydwe::windows_exception ex(nullptr, ERROR_FILE_NOT_FOUND);
				if (!ec) {
					throw ex;
				}
				else {
					*ec = ex.code();
					return ;
				}
			}
		}

		void zip_reader_iterator_increment(ZipReader::iterator& it)
		{
			assert(it.impl_.get());

			if (unzGoToNextFile(it.impl_->entry_.zip_file()) != UNZ_OK)
			{
				it.impl_.reset();
			}
		}

		ZipReader::entry& zip_reader_iterator_dereference(const ZipReader::iterator& it, std::error_code* ec)
		{
			assert(it.impl_.get());

			unz_file_info raw_file_info = {};
			char raw_file_name_in_zip[internal::kZipMaxPath] = {};
			const int result = unzGetCurrentFileInfo(it.impl_->entry_.zip_file(),
				&raw_file_info,
				raw_file_name_in_zip,
				sizeof(raw_file_name_in_zip) - 1,
				NULL,  // extraField.
				0,  // extraFieldBufferSize.
				NULL,  // szComment.
				0);  // commentBufferSize.
			if (result != UNZ_OK)
				throw false;
			if (raw_file_name_in_zip[0] == '\0')
				throw false;

			return it.impl_->entry_.assign(raw_file_name_in_zip, raw_file_info);
		}
	}

	ZipReader::entry::entry(const unzFile* zf_ptr)
		: zf_ptr_(zf_ptr)
	{ }

	// TODO(satorux): The implementation assumes that file names in zip files
	// are encoded in UTF-8. This is true for zip files created by Zip()
	// function in zip.h, but not true for user-supplied random zip files.
	ZipReader::entry& ZipReader::entry::assign(const std::string& file_name_in_zip, const unz_file_info& raw_file_info)
	{
		file_path_ = file_name_in_zip;
		is_directory_ = false;

		original_size_ = raw_file_info.uncompressed_size;

		// Directory entries in zip files end with "/".
		is_directory_ = file_name_in_zip.back() == '/';

		// Check the file name here for directory traversal issues. In the name of
		// simplicity and security, we might reject a valid file name such as "a..b".
		is_unsafe_ = file_name_in_zip.find("..") != std::string::npos;

		// 			// We also consider that the file name is unsafe, if it's invalid UTF-8.
		// 			std::wstring file_name_utf16;
		// 			if (!UTF8ToUTF16(file_name_in_zip.data(), file_name_in_zip.size(), &file_name_utf16)) {
		// 				is_unsafe_ = true;
		// 			}
		// 
		// 			// We also consider that the file name is unsafe, if it's absolute.
		// 			// On Windows, IsAbsolute() returns false for paths starting with "/".
		// 			if (file_path_.IsAbsolute() || StartsWithASCII(file_name_in_zip, "/", false))
		// 				is_unsafe_ = true;

		return *this;
	}

	bool ZipReader::entry::extract_to_buffer(void* buf, size_t len) 
	{
		if (is_directory())
			return false;

		const int open_result = unzOpenCurrentFile(zip_file());
		if (open_result != UNZ_OK)
			return false;

		bool success = true;

		const int num_bytes_read = unzReadCurrentFile(zip_file(), buf, len);
		if (num_bytes_read == 0) 
		{
			// Reached the end of the file.
		} 
		else if (num_bytes_read < 0) 
		{
			// If num_bytes_read < 0, then it's a specific UNZ_* error code.
			success = false;
		} 

		unzCloseCurrentFile(zip_file());
		return success;
	}

	ZipReader::ZipReader() 
	{
		Reset();
	}

	ZipReader::~ZipReader() 
	{
		Close();
	}

	bool ZipReader::Open(const boost::filesystem::path& zip_file_path) 
	{
		// Use of "Unsafe" function does not look good, but there is no way to do
		// this safely on Linux. See file_util.h for details.
		zip_file_ = internal::OpenForUnzipping(zip_file_path.string(boost::filesystem::detail::utf8_codecvt_facet()));
		if (!zip_file_)
		{
			return false;
		}

		return OpenInternal();
	}

	bool ZipReader::OpenFromString(const std::string& data)
	{
		zip_file_ = internal::PreprareMemoryForUnzipping(data);
		if (!zip_file_)
		{
			return false;
		}

		return OpenInternal();
	}

	void ZipReader::Close() 
	{
		if (zip_file_) 
		{
			unzClose(zip_file_);
		}
		Reset();
	}

	bool ZipReader::LocateAndOpenEntry(const boost::filesystem::path& path_in_zip) 
	{
		const int kDefaultCaseSensivityOfOS = 0;
		const int result = unzLocateFile(zip_file_, path_in_zip.string(boost::filesystem::detail::utf8_codecvt_facet()).c_str(), kDefaultCaseSensivityOfOS);
		if (result != UNZ_OK)
			return false;

		// Then Open the entry.
		//return OpenCurrentEntryInZip();
		return false;
	}

	bool ZipReader::OpenInternal() 
	{
		unz_global_info zip_info = {};  // Zero-clear.
		if (unzGetGlobalInfo(zip_file_, &zip_info) != UNZ_OK) 
		{
			return false;
		}
		return true;
	}

	void ZipReader::Reset() 
	{
		zip_file_ = NULL;
	}

	ZipReader::iterator ZipReader::begin() const
	{
		return iterator(&zip_file_);
	}

	ZipReader::iterator ZipReader::end() const
	{
		return iterator();
	}
}
