#include "zip_reader.h"
#include "zip_internal.h"

#include <fstream>
#include <boost/filesystem/detail/utf8_codecvt_facet.hpp>

#include <minizip/unzip.h>
#include <minizip/iowin32.h>

namespace zip {

	// TODO(satorux): The implementation assumes that file names in zip files
	// are encoded in UTF-8. This is true for zip files created by Zip()
	// function in zip.h, but not true for user-supplied random zip files.
	ZipReader::EntryInfo::EntryInfo(const std::string& file_name_in_zip, const unz_file_info& raw_file_info)
		: file_path_(file_name_in_zip)
		, is_directory_(false)
	{
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
			return false;
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

	bool ZipReader::HasMore() 
	{
		return !reached_end_;
	}

	bool ZipReader::AdvanceToNextEntry() 
	{
		// Should not go further if we already reached the end.
		if (reached_end_)
			return false;

		unz_file_pos position = {};
		if (unzGetFilePos(zip_file_, &position) != UNZ_OK)
			return false;
		const int current_entry_index = position.num_of_file;
		// If we are currently at the last entry, then the next position is the
		// end of the zip file, so mark that we reached the end.
		if (current_entry_index + 1 == num_entries_) 
		{
			reached_end_ = true;
		}
		else 
		{
			if (unzGoToNextFile(zip_file_) != UNZ_OK) 
			{
				return false;
			}
		}
		current_entry_info_.reset();
		return true;
	}

	bool ZipReader::OpenCurrentEntryInZip() 
	{
		unz_file_info raw_file_info = {};
		char raw_file_name_in_zip[internal::kZipMaxPath] = {};
		const int result = unzGetCurrentFileInfo(zip_file_,
			&raw_file_info,
			raw_file_name_in_zip,
			sizeof(raw_file_name_in_zip) - 1,
			NULL,  // extraField.
			0,  // extraFieldBufferSize.
			NULL,  // szComment.
			0);  // commentBufferSize.
		if (result != UNZ_OK)
			return false;
		if (raw_file_name_in_zip[0] == '\0')
			return false;
		current_entry_info_.reset(new EntryInfo(raw_file_name_in_zip, raw_file_info));
		return true;
	}

	bool ZipReader::LocateAndOpenEntry(const boost::filesystem::path& path_in_zip) 
	{
		current_entry_info_.reset();
		reached_end_ = false;
		const int kDefaultCaseSensivityOfOS = 0;
		const int result = unzLocateFile(zip_file_, path_in_zip.string(boost::filesystem::detail::utf8_codecvt_facet()).c_str(), kDefaultCaseSensivityOfOS);
		if (result != UNZ_OK)
			return false;

		// Then Open the entry.
		return OpenCurrentEntryInZip();
	}

	bool ZipReader::ExtractCurrentEntryToFilePath(const boost::filesystem::path& output_file_path) 
	{
		// If this is a directory, just create it and return.
		if (current_entry_info()->is_directory())
			return boost::filesystem::create_directories(output_file_path);

		const int open_result = unzOpenCurrentFile(zip_file_);
		if (open_result != UNZ_OK)
			return false;

		// We can't rely on parent directory entries being specified in the
		// zip, so we make sure they are created.
		boost::filesystem::path output_dir_path = output_file_path.parent_path();
		if (!boost::filesystem::create_directories(output_dir_path))
			return false;

		std::fstream stream(output_file_path.wstring(), std::ios::binary | std::ios_base::out);
		if (!stream.is_open())
			return false;

		bool success = true;  // This becomes false when something bad happens.
		while (true) 
		{
			char buf[internal::kZipBufSize];
			const int num_bytes_read = unzReadCurrentFile(zip_file_, buf, internal::kZipBufSize);
			if (num_bytes_read == 0) 
			{
				// Reached the end of the file.
				break;
			} 
			else if (num_bytes_read < 0) 
			{
				// If num_bytes_read < 0, then it's a specific UNZ_* error code.
				success = false;
				break;
			} 
			else if (num_bytes_read > 0) 
			{
				// Some data is read. Write it to the output file.
				stream.write(buf, num_bytes_read);
				if (num_bytes_read != stream.gcount()) 
				{
					success = false;
					break;
				}
			}
		}

		unzCloseCurrentFile(zip_file_);
		return success;
	}

	bool ZipReader::ExtractCurrentEntryIntoDirectory(const boost::filesystem::path& output_directory_path) 
	{
		boost::filesystem::path output_file_path = output_directory_path / current_entry_info()->file_path();
		return ExtractCurrentEntryToFilePath(output_file_path);
	}

	bool ZipReader::ExtractCurrentEntryToBuffer(void* buf, size_t len) 
	{
		if (current_entry_info()->is_directory())
			return false;

		const int open_result = unzOpenCurrentFile(zip_file_);
		if (open_result != UNZ_OK)
			return false;

		bool success = true;

		const int num_bytes_read = unzReadCurrentFile(zip_file_, buf, len);
		if (num_bytes_read == 0) 
		{
			// Reached the end of the file.
		} 
		else if (num_bytes_read < 0) 
		{
			// If num_bytes_read < 0, then it's a specific UNZ_* error code.
			success = false;
		} 

		unzCloseCurrentFile(zip_file_);
		return success;
	}

	bool ZipReader::OpenInternal() 
	{
		unz_global_info zip_info = {};  // Zero-clear.
		if (unzGetGlobalInfo(zip_file_, &zip_info) != UNZ_OK) 
		{
			return false;
		}
		num_entries_ = zip_info.number_entry;
		if (num_entries_ < 0)
			return false;

		// We are already at the end if the zip file is empty.
		reached_end_ = (num_entries_ == 0);
		return true;
	}

	void ZipReader::Reset() 
	{
		zip_file_ = NULL;
		num_entries_ = 0;
		reached_end_ = false;
		current_entry_info_.reset();
	}
}
