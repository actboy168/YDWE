#pragma once

#include <string>
#include <boost/filesystem.hpp>

#include <minizip/unzip.h>

#if defined(YDWE_ZIP_EXPORTS)
#	define _ZIP_API __declspec(dllexport)
#else
#	define _ZIP_API __declspec(dllimport)
#endif

namespace zip 
{
	class _ZIP_API ZipReader 
	{
	public:
		class EntryInfo 
		{
		public:
			EntryInfo(const std::string& filename_in_zip, const unz_file_info& raw_file_info);

			const boost::filesystem::path& file_path() const { return file_path_; }
			uint64_t original_size() const { return original_size_; }
			bool is_directory() const { return is_directory_; }
			bool is_unsafe() const { return is_unsafe_; }

		private:
			const boost::filesystem::path file_path_;
			uint64_t original_size_;
			bool is_directory_;
			bool is_unsafe_;

			EntryInfo(const EntryInfo&);
			void operator=(const EntryInfo&);
		};

		ZipReader();
		~ZipReader();

		bool Open(const boost::filesystem::path& zip_file_path);
		bool OpenFromString(const std::string& data);
		void Close();
		bool HasMore();
		bool AdvanceToNextEntry();
		bool OpenCurrentEntryInZip();
		bool LocateAndOpenEntry(const boost::filesystem::path& path_in_zip);
		bool ExtractCurrentEntryToFilePath(const boost::filesystem::path& output_file_path);
		bool ExtractCurrentEntryIntoDirectory(const boost::filesystem::path& output_directory_path);
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
#pragma warning(suppress:4251)
		std::unique_ptr<EntryInfo> current_entry_info_;

		ZipReader(const ZipReader&);
		void operator=(const ZipReader&);
	};

}
