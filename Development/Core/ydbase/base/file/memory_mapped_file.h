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
		~memory_mapped_file();
		void const* data() const;
		size_t size() const;

	private:
		void*  data_;
		size_t size_;
	};
#pragma warning(pop)
}
}
