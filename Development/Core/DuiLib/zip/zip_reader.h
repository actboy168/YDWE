#pragma once

#include <base/filesystem.h>
#include <string>				
#include <iterator>
#include <unzip.h>

namespace zip 
{
	class reader 
	{
	public:
		reader();
		~reader();

		bool Open(const fs::path& zip_file_path);
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
		const fs::path& file_path() const { assert(is_vaild()); return file_path_; }
		uint64_t original_size() const { assert(is_vaild()); return original_size_; }
		bool is_directory() const { assert(is_vaild()); return is_directory_; }
		bool is_unsafe() const { assert(is_vaild()); return is_unsafe_; }
		bool extract_to_buffer(void* buf, size_t len);

	private:
		const unzFile* zf_ptr_;
		fs::path file_path_;
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
		: public std::iterator<std::input_iterator_tag, entry>
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

		reference operator*() const
		{
			return detail::zip_reader_iterator_dereference(*this);
		}

		iterator operator++(int)
		{
			auto result = *this;
			++(*this);
			return result;
		}

		iterator& operator++()
		{
			detail::zip_reader_iterator_increment(*this);
			return *this;
		}

		bool operator==(const iterator& other) const
		{
			return impl_ == other.impl_;
		}

		bool operator!=(const iterator& other) const
		{
			return !operator==(other);
		}

	private:
		friend void detail::zip_reader_iterator_construct(iterator& it, const unzFile* zf_ptr, std::error_code* ec);
		friend void detail::zip_reader_iterator_increment(iterator& it);
		friend entry& detail::zip_reader_iterator_dereference(const iterator& it);

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
