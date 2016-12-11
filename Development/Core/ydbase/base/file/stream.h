#pragma once

#include <base/filesystem.h>
#include <base/config.h>
#include <base/util/noncopyable.h>
#include <fstream>

namespace base { 
namespace file {

#pragma warning(push)
#pragma warning(disable:4275)

	class _BASE_API stream
		: private noncopyable
	{
	public:
		stream(const char* filename, std::ios_base::openmode mode);
		stream(const wchar_t* filename, std::ios_base::openmode mode);
		~stream();
		bool is_open() const;

		template <class SequenceT>
		SequenceT read()
		{
			return std::move(SequenceT((std::istreambuf_iterator<char>(file_)), (std::istreambuf_iterator<char>())));
		}

		template <class SequenceT>
		void write(SequenceT buf)
		{
			std::copy(buf.begin(), buf.end(), std::ostreambuf_iterator<char>(file_));
		}

	private:
		stream();

#pragma warning(push)
#pragma warning(disable:4251)
		std::fstream file_;
#pragma warning(pop)
	};

	class _BASE_API write_stream
		: private noncopyable
	{
	public:
		write_stream(const char* filename);
		write_stream(const std::string& filename);
		write_stream(const wchar_t* filename);
		write_stream(const std::wstring& filename);
		write_stream(const fs::path& filename);
		
		template <class SequenceT>
		void write(SequenceT buf)
		{
			file_.write<SequenceT>(std::forward<SequenceT>(buf));
		}

	private:
		stream file_;
	};

	class _BASE_API read_stream
		: private noncopyable
	{
	public:
		read_stream(const char* filename);
		read_stream(const std::string& filename);
		read_stream(const wchar_t* filename);
		read_stream(const std::wstring& filename);
		read_stream(const fs::path& filename);

		template <class SequenceT>
		SequenceT read()
		{
			return std::move(file_.read<SequenceT>());
		}

	private:
		stream file_;
	};

#pragma warning(pop)
#pragma warning(disable:4275)
}
}
