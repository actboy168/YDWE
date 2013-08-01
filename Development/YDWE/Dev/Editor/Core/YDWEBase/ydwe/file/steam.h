#pragma once

#include <boost/filesystem.hpp>
#include <ydwe/config.h>
#include <ydwe/util/noncopyable.h>
#include <fstream>

_BASE_BEGIN namespace file {

	class _BASE_API steam: public util::noncopyable
	{
	public:
		steam(const char* filename, std::ios_base::openmode mode);
		steam(const wchar_t* filename, std::ios_base::openmode mode);
		~steam();
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
		steam();

#pragma warning(push)
#pragma warning(disable:4251)
		std::fstream file_;
#pragma warning(pop)
	};

	class _BASE_API write_steam
	{
	public:
		write_steam(const char* filename);
		write_steam(const std::string& filename);
		write_steam(const wchar_t* filename);
		write_steam(const std::wstring& filename);
		write_steam(const boost::filesystem::path& filename);
		
		template <class SequenceT>
		void write(SequenceT buf)
		{
			file_.write<SequenceT>(std::forward<SequenceT>(buf));
		}

	private:
		steam file_;
	};

	class _BASE_API read_steam
	{
	public:
		read_steam(const char* filename);
		read_steam(const std::string& filename);
		read_steam(const wchar_t* filename);
		read_steam(const std::wstring& filename);
		read_steam(const boost::filesystem::path& filename);

		template <class SequenceT>
		SequenceT read()
		{
			return std::move(file_.read<SequenceT>());
		}

	private:
		steam file_;
	};
}}
