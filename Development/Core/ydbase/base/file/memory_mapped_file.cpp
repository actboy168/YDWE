#include <base/file/memory_mapped_file.h>
#include <base/file/file_handle.h>
#include <base/exception/windows_exception.h>

namespace base { namespace file {
	memory_mapped_file::memory_mapped_file(const wchar_t* file_name)
		: size_(0)
		, data_(NULL)
	{
		file_handle file_(file_name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
		uint64_t file_size = file_.get_size();
		if (0 == file_size) {
			return;
		}
#if defined(_WIN64)
		size_ = file_size;
#else
		if ((file_size >> 32) > 0) {
			throw windows_exception("region size too large", ERROR_NOT_ENOUGH_MEMORY);
		}
		size_ = file_size & 0xFFFFFFFF;
#endif
		win::scoped_handle<NULL> map_(::CreateFileMappingW(file_.get(), NULL, PAGE_READONLY, file_size >> 32, file_size & 0xFFFFFFFF, NULL));
		if (!map_) {
			throw windows_exception("failed to open file mapping");
		}
		data_ = ::MapViewOfFile(map_.get(), FILE_MAP_READ, 0, 0, size_);
		if (!data_) {
			throw windows_exception("failed to map view of file");
		}
	}

	memory_mapped_file::~memory_mapped_file() 
	{
		if (data_) {
			::UnmapViewOfFile(data_);
		}
	}

	void const* memory_mapped_file::data() const
	{
		return data_;
	}

	size_t memory_mapped_file::size() const
	{
		return size_;
	}
}}
