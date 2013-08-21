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

		enum
		{
			normal = 0,
			stream_eof,
		};

		buffer_reader(const buffer& b)
			: beg_(b.begin())
			, end_(b.end())
			, cur_(beg_)
		{ }

		buffer_reader& operator=(const buffer_reader& source)
		{
			if (this != &source)
			{
				beg_ = source.beg_;
				cur_ = source.cur_;
				end_ = source.end_;
			}

			return *this;
		}

		buffer_reader& operator=(buffer_reader&& source)
		{
			if (this != &source)
			{
				beg_ = source.beg_;
				cur_ = source.cur_;
				end_ = source.end_;
			}

			return *this;
		}

		void reset()
		{
			cur_ = beg_;
		}

		const value_type* reads_ptr(size_t n)
		{
			if (static_cast<size_type>(end_ - cur_) < n)
			{
				throw exception("buffer overflow.");
			}

			const value_type* ret = &*cur_;
			cur_ += n;

			return ret;
		}

		template <class _Ty>
		const _Ty* read_ptr()
		{
			return reinterpret_cast<const _Ty*>(reads_ptr(sizeof(_Ty)));
		}

		template <class _Ty>
		_Ty read()
		{
			return * read_ptr<_Ty>();
		}

		const value_type* reads_ptr(size_t n, std::error_code& ec)
		{
			if (end_ - cur_ <  static_cast<buffer_type::difference_type>(n))
			{
				ec.assign(stream_eof, std::system_category());
				return nullptr;
			}

			const value_type* ret = &*cur_;
			cur_ += n;

			return ret;
		}

		template <class _Ty>
		const _Ty* read_ptr(std::error_code& ec)
		{
			return reinterpret_cast<const _Ty*>(reads_ptr(sizeof(_Ty), ec));
		}

		template <class _Ty>
		_Ty read(std::error_code& ec)
		{
			const _Ty* p = read_ptr<_Ty>(ec);
			return p ? *p : _Ty();
		}

		template <>
		std::string read<std::string>(std::error_code& ec)
		{
			const_iterator start = cur_;
			value_type c = read<value_type>(ec);
			while (!ec && c != '\0') { c = read<value_type>(ec); }
			return std::move(std::string(start, cur_));
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
					cur_ = beg_ + offset;
				}
				break;
			case  std::ios::cur:
				{
					if (static_cast<size_type>(end_ - cur_) < offset)
					{
						throw exception("buffer overflow.");
					}
					cur_ = cur_ + offset;
				}
				break;
			case  std::ios::end:
				{
					if (size() < offset)
					{
						throw exception("buffer overflow.");
					}
					cur_ = beg_ + (size() - offset);
				}
				break;
			}
		}

		size_type size() const
		{
			return static_cast<size_type>(end_ - beg_);
		}

	private:
		const_iterator beg_;
		const_iterator end_;
		const_iterator cur_;
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
