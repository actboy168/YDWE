#include "zip_reader.h"
#include "zip_internal.h"

#include <fstream>
#include <base/exception/windows_exception.h>	   
#include <base/util/unicode.h>
#include <unzip.h>
#include <iowin32.h>

namespace zip {

	namespace detail
	{
		void zip_reader_iterator_construct(reader::iterator& it, const unzFile* zf_ptr, std::error_code* ec)
		{
			if (unzGoToFirstFile(*zf_ptr) != UNZ_OK)
			{
				it.impl_.reset();

				base::windows_exception ex(nullptr, ERROR_FILE_NOT_FOUND);
				if (!ec) {
					throw ex;
				}
				else {
					*ec = ex.code();
					return ;
				}
			}
		}

		void zip_reader_iterator_increment(reader::iterator& it)
		{
			assert(it.impl_.get());

			if (unzGoToNextFile(it.impl_->entry_.zip_file()) != UNZ_OK)
			{
				it.impl_.reset();
			}
		}

		reader::entry& zip_reader_iterator_dereference(const reader::iterator& it)
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
			{
				return it.impl_->entry_.assign();
			}

			if (raw_file_name_in_zip[0] == '\0')
			{
				return it.impl_->entry_.assign();
			}

			return it.impl_->entry_.assign(raw_file_name_in_zip, raw_file_info);
		}
	}

	reader::entry::entry(const unzFile* zf_ptr)
		: zf_ptr_(zf_ptr)
	{ }

	reader::entry& reader::entry::assign()
	{
		file_path_.clear();
		return *this;
	}

	// TODO(satorux): The implementation assumes that file names in zip files
	// are encoded in UTF-8. This is true for zip files created by Zip()
	// function in zip.h, but not true for user-supplied random zip files.
	reader::entry& reader::entry::assign(const std::string& file_name_in_zip, const unz_file_info& raw_file_info)
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

	bool reader::entry::extract_to_buffer(void* buf, size_t len) 
	{
		assert(is_vaild());

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

	reader::reader() 
	{
		Reset();
	}

	reader::~reader() 
	{
		Close();
	}

	bool reader::Open(const fs::path& zip_file_path) 
	{
		// Use of "Unsafe" function does not look good, but there is no way to do
		// this safely on Linux. See file_util.h for details.
		zip_file_ = internal::OpenForUnzipping(base::w2u(zip_file_path.wstring()));
		if (!zip_file_)
		{
			return false;
		}

		return true;
	}

	bool reader::OpenFromString(const std::string& data)
	{
		zip_file_ = internal::PreprareMemoryForUnzipping(data);
		if (!zip_file_)
		{
			return false;
		}

		return true;
	}

	void reader::Close() 
	{
		if (zip_file_) 
		{
			unzClose(zip_file_);
		}
		Reset();
	}

	void reader::Reset() 
	{
		zip_file_ = NULL;
	}

	reader::iterator reader::begin() const
	{
		return iterator(&zip_file_);
	}

	reader::iterator reader::end() const
	{
		return iterator();
	}
}
