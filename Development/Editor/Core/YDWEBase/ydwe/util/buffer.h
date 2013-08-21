#pragma once

#include <ydwe/exception/exception.h>
#include <ydwe/util/noncopyable.h>
#include <string>
#include <system_error>
#include <vector>
#include <streambuf>

_BASE_BEGIN 
namespace util {

	class buffer
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
		buffer(Elem const* from, size_t length)
			: buf_(static_cast<value_type const*>(from), static_cast<value_type const*>(from)+length)
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
	{
	public:
		typedef buffer::buffer_type         buffer_type;
		typedef buffer_type::iterator       iterator;
		typedef buffer_type::const_iterator const_iterator;
		typedef buffer_type::value_type     value_type;
		typedef buffer_type::size_type      size_type;
		typedef buffer_type::pointer        pointer;
		typedef buffer_type::const_pointer  const_pointer;

		enum
		{
			normal = 0,
			stream_eof,
		};

		buffer_reader(const buffer& b)
			: first_(&*b.begin())
			, last_(&*b.begin() + b.size())
			, next_(first_)
		{ }

		buffer_reader& operator=(const buffer_reader& source)
		{
			if (this != &source)
			{
				first_ = source.first_;
				next_ = source.next_;
				last_ = source.last_;
			}

			return *this;
		}

		buffer_reader& operator=(buffer_reader&& source)
		{
			if (this != &source)
			{
				first_ = source.first_;
				next_ = source.next_;
				last_ = source.last_;
			}

			return *this;
		}

		void reset()
		{
			next_ = first_;
		}

		const value_type* reads_ptr(size_t n)
		{
			if (static_cast<size_type>(last_ - next_) < n)
			{
				throw exception("buffer overflow.");
			}

			const value_type* ret = &*next_;
			next_ += n;

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

		const value_type* reads_ptr(size_t n, std::error_code& ec)
		{
			if (last_ - next_ <  static_cast<buffer_type::difference_type>(n))
			{
				ec.assign(stream_eof, std::generic_category());
				return nullptr;
			}

			const value_type* ret = &*next_;
			next_ += n;

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
					if (size() < offset)
					{
						throw exception("buffer overflow.");
					}
					next_ = first_ + offset;
				}
				break;
			case  std::ios::cur:
				{
					if (static_cast<size_type>(last_ - next_) < offset)
					{
						throw exception("buffer overflow.");
					}
					next_ = next_ + offset;
				}
				break;
			case  std::ios::end:
				{
					if (size() < offset)
					{
						throw exception("buffer overflow.");
					}
					next_ = first_ + (size() - offset);
				}
				break;
			}
		}

		size_type size() const
		{
			return static_cast<size_type>(last_ - first_);
		}

	private:
		const_pointer first_;
		const_pointer last_;
		const_pointer next_;
	};

	class buffer_stearmbuf 
		: public std::streambuf 
		, public util::noncopyable
	{
	public:
		buffer_stearmbuf(buffer& b)
		{
			setg(&*b.begin(), &*b.begin(), &*b.begin() + b.size());
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
}
_BASE_END
