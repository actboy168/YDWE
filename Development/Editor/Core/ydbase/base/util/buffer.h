#pragma once

#include <base/exception/exception.h>
#include <base/util/noncopyable.h>
#include <string>
#include <system_error>
#include <vector>
#include <streambuf>

namespace base { 

	class buffer
		: private noncopyable
	{
	public:
		typedef std::vector<char>           buffer_type;
		typedef buffer_type::iterator       iterator;
		typedef buffer_type::const_iterator const_iterator;
		typedef buffer_type::value_type     value_type;
		typedef buffer_type::size_type      size_type;
		typedef buffer_type::pointer        pointer;
		typedef buffer_type::const_pointer  const_pointer;

		static_assert(sizeof(value_type) == 1, "buffer::value_type's size must be 1.");

	protected:
		buffer_type buf_;

	public:
		buffer()
			: buf_()
		{ }

		buffer(buffer&& source)
			: buf_(std::move(source.buf_))
		{ }

		template <class Source>
		buffer(Source const& source)
			: buf_(source.begin(), source.end())
		{ }

		template <class InputIterator>
		buffer(InputIterator begin, InputIterator end)
			: buf_(begin, end)
		{ }

		template <class Elem>
		buffer(Elem const* from, size_t from_size)
			: buf_(static_cast<const value_type*>(from), static_cast<const value_type*>(from)+from_size)
		{ }

		~buffer()
		{ }

		iterator begin()
		{
			return buf_.begin();
		}

		const_iterator begin() const
		{
			return buf_.begin();
		}

		iterator end()
		{
			return buf_.end();
		}

		const_iterator end() const
		{
			return buf_.end();
		}

		size_type size() const
		{
			return buf_.size();
		}
	};

	class buffer_reader
		: private noncopyable
	{
	public:
		typedef buffer::buffer_type         buffer_type;
		typedef buffer_type::value_type     value_type;
		typedef buffer_type::size_type      size_type;
		typedef buffer_type::pointer        pointer;
		typedef buffer_type::const_pointer  const_pointer;

		buffer_reader(buffer& b)
			: first_(&*b.begin())
			, next_(first_)
			, max_size_(b.size())
			, size_(max_size_)
		{ }

		void reset()
		{
			next_ = first_;
		}

		const value_type* reads_ptr(size_type n)
		{
			if (size_ < n)
			{
				throw exception("buffer overflow.");
			}

			const value_type* ret = &*next_;
			next_ += n;
			size_ -= n;

			return ret;
		}

		template <class T>
		const T* read_ptr()
		{
			return reinterpret_cast<const T*>(reads_ptr(sizeof(T)));
		}

		template <class T>
		T read()
		{
			return * read_ptr<T>();
		}

		const value_type* reads_ptr(size_type n, std::error_code& ec)
		{
			if (size_ < n)
			{
				ec.assign(ENOMEM, std::generic_category());
				return nullptr;
			}

			const value_type* ret = &*next_;
			next_ += n;
			size_ -= n;

			return ret;
		}

		template <class T>
		const T* read_ptr(std::error_code& ec)
		{
			return reinterpret_cast<const T*>(reads_ptr(sizeof(T), ec));
		}

		template <class T>
		T read(std::error_code& ec)
		{
			const T* p = read_ptr<T>(ec);
			return p ? *p : T();
		}

		template <>
		std::string read<std::string>(std::error_code& ec)
		{
			const_pointer start = next_;
			value_type c = read<value_type>(ec);
			while (!ec && c != '\0') { c = read<value_type>(ec); }
			return std::move(std::string(start, next_));
		}

		template <>
		std::string read<std::string>()
		{
			std::error_code ec;
			return std::move(read<std::string>(ec));
		}

		void seek(size_t offset, std::ios::seek_dir dir)
		{
			switch (dir)
			{
			case  std::ios::beg:
				{
					if (max_size_ < offset)
					{
						throw exception("buffer overflow.");
					}
					next_ = first_ + offset;
				}
				break;
			case  std::ios::cur:
				{
					if (size_ < offset)
					{
						throw exception("buffer overflow.");
					}
					next_ = next_ + offset;
				}
				break;
			case  std::ios::end:
				{
					if (max_size_ < offset)
					{
						throw exception("buffer overflow.");
					}
					next_ = first_ + (max_size_ - offset);
				}
				break;
			}
		}

	private:
		const_pointer first_;
		const_pointer next_;
		size_type     max_size_;
		size_type     size_;
	};

	class buffer_streambuf 
		: public std::streambuf
		, private noncopyable
	{
	public:
		buffer_streambuf(buffer& b)
		{
			setg(&*b.begin(), &*b.begin(), &*b.begin() + b.size());
		}

		buffer_streambuf(char* first, char* last)
		{ 
			setg(first, first, last);
		}

		virtual std::streambuf::int_type underflow() 
		{
			if (gptr() < egptr())
			{
				return traits_type::to_int_type(*gptr());
			}

			return traits_type::eof(); 
		}
	};
	  	   
#pragma warning(push)
#pragma warning(disable: 4250)

	class buffer_stream 
		: public std::iostream 
		, private noncopyable
	{
	public:
		buffer_stream(buffer& b) 
			: std::iostream(new buffer_streambuf(b)) 
		{ }

		buffer_stream(char* first, char* last) 
			: std::iostream(new buffer_streambuf(first, last)) 
		{ }

		~buffer_stream() 
		{
			delete rdbuf();
		}
	};		 
#pragma warning(pop)
}
