#pragma once

#include <base/config.h>
#include <base/util/noncopyable.h>
#include <cstdint>

namespace base { 
namespace file {

#pragma warning(push)
#pragma warning(disable:4275)
	class _BASE_API memory_mapped_file 
		: private noncopyable
	{
	public:
		memory_mapped_file(const wchar_t* file_name);
		memory_mapped_file(const wchar_t* file_name, uint64_t offset, size_t request_size);
		memory_mapped_file(void* file, uint64_t offset, size_t request_size);
		~memory_mapped_file();
		void const* memory() const;
		size_t size() const;

	private:
		void open(const wchar_t* file_name, uint64_t offset, size_t request_size);
		void open(void* file, uint64_t offset, size_t request_size);

	private:
		size_t           cb_;
		void*            memory_;
	};
#pragma warning(pop)
}
}
