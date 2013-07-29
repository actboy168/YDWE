#pragma once

#include <ydwe/config.h>
#include <ydwe/util/noncopyable.h>
#include <cstdint>

namespace ydwe { namespace file {

	class YDWE_BASE_API memory_mapped_file : public util::noncopyable
	{
	public:
		memory_mapped_file(const wchar_t* file_name);
		memory_mapped_file(const wchar_t* file_name, uint64_t offset, size_t request_size);
		~memory_mapped_file();
		void const* memory() const;
		size_t size() const;

	private:
		void open(const wchar_t* file_name, uint64_t offset, size_t request_size);

	private:
		size_t           cb_;
		void*            memory_;
	};
}}
