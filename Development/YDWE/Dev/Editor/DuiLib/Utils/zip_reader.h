#pragma once

#include <string>
#include <boost/filesystem.hpp>
namespace fs = ::boost::filesystem;

#include <minizip/unzip.h>

namespace zip 
{
	class ZipReader 
	{
	public:
		class EntryInfo 
		{
		public:
			EntryInfo(const std::string& filename_in_zip, const unz_file_info& raw_file_info);

			const fs::path& file_path() const { return file_path_; }
			uint64_t original_size() const { return original_size_; }
			bool is_directory() const { return is_directory_; }
			bool is_unsafe() const { return is_unsafe_; }

		private:
			const fs::path file_path_;
			uint64_t original_size_;
			bool is_directory_;
			bool is_unsafe_;

			EntryInfo(const EntryInfo&);
			void operator=(const EntryInfo&);
		};

		ZipReader();
		~ZipReader();

		bool Open(const fs::path& zip_file_path);
		bool OpenFromString(const std::string& data);
		void Close();
		bool HasMore();
		bool AdvanceToNextEntry();
		bool OpenCurrentEntryInZip();
		bool LocateAndOpenEntry(const fs::path& path_in_zip);
		bool ExtractCurrentEntryToFilePath(const fs::path& output_file_path);
		bool ExtractCurrentEntryIntoDirectory(const fs::path& output_directory_path);
		bool ExtractCurrentEntryToBuffer(void* buf, size_t len);
		EntryInfo* current_entry_info() const 
		{
			return current_entry_info_.get();
		}

		int num_entries() const { return num_entries_; }

	private:
		bool OpenInternal();
		void Reset();

		unzFile zip_file_;
		int num_entries_;
		bool reached_end_;
		std::unique_ptr<EntryInfo> current_entry_info_;

		ZipReader(const ZipReader&);
		void operator=(const ZipReader&);
	};

}
