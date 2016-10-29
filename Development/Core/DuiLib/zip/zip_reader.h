#pragma once

#include <string>				
#include <base/filesystem.h>
#include <boost/iterator.hpp>

#include <unzip.h>

#if defined(YDWE_ZIP_EXPORTS)
#	define _ZIP_API __declspec(dllexport)
#else
#	define _ZIP_API __declspec(dllimport)
#endif

namespace zip 
{
	class reader 
	{
	public:
		reader();
		~reader();

		bool Open(const boost::filesystem::path& zip_file_path);
		bool OpenFromString(const std::string& data);
		void Close();

		class entry;
		class iterator;
		typedef iterator const_iterator;
		iterator begin() const;
		iterator end() const;

	private:
		void Reset();

		unzFile zip_file_;

		reader(const reader&);
		void operator=(const reader&);
	};

	class reader::entry
	{
	public:
		entry(const unzFile* zf_ptr);
		entry& assign();
		entry& assign(const std::string& filename_in_zip, const unz_file_info& raw_file_info);

		bool is_vaild() const { return !file_path_.empty(); }
		const unzFile& zip_file() const { assert(zf_ptr_ != nullptr); return *zf_ptr_; }
		const boost::filesystem::path& file_path() const { assert(is_vaild()); return file_path_; }
		uint64_t original_size() const { assert(is_vaild()); return original_size_; }
		bool is_directory() const { assert(is_vaild()); return is_directory_; }
		bool is_unsafe() const { assert(is_vaild()); return is_unsafe_; }
		bool extract_to_buffer(void* buf, size_t len);

	private:
		const unzFile* zf_ptr_;
		boost::filesystem::path file_path_;
		uint64_t original_size_;
		bool     is_directory_;
		bool     is_unsafe_;

		entry(const entry&);
		void operator=(const entry&);
	};

	namespace detail
	{
		void zip_reader_iterator_construct(reader::iterator& it, const unzFile* zf_ptr, std::error_code* ec);
		void zip_reader_iterator_increment(reader::iterator& it);
		reader::entry& zip_reader_iterator_dereference(const reader::iterator& it);
	}

	class reader::iterator
		: public boost::iterator_facade<iterator, entry, boost::single_pass_traversal_tag>
	{
	public:
		iterator()
			: impl_()
		{ }

		iterator(const unzFile* zf_ptr)
			: impl_(new iterator_impl(zf_ptr))
		{ 
			detail::zip_reader_iterator_construct(*this, zf_ptr, 0);
		}

		iterator(const unzFile* zf_ptr, std::error_code& ec)
			: impl_(new iterator_impl(zf_ptr))
		{ 
			detail::zip_reader_iterator_construct(*this, zf_ptr, &ec); 
		}

		~iterator() 
		{ }

	private:
		friend class boost::iterator_core_access;
		friend void detail::zip_reader_iterator_construct(iterator& it, const unzFile* zf_ptr, std::error_code* ec);
		friend void detail::zip_reader_iterator_increment(iterator& it);
		friend entry& detail::zip_reader_iterator_dereference(const iterator& it);

		void   increment() 
		{
			detail::zip_reader_iterator_increment(*this);
		}

		bool   equal(const iterator& other) const
		{
			return impl_ == other.impl_;
		}

		entry& dereference() const
		{
			return detail::zip_reader_iterator_dereference(*this);
		}

		struct iterator_impl
		{
			iterator_impl(const unzFile* zf_ptr)
				: entry_(zf_ptr)
			{ }

			entry          entry_;
		};
#pragma warning(suppress:4251)
		std::shared_ptr<iterator_impl> impl_;
	};
}
