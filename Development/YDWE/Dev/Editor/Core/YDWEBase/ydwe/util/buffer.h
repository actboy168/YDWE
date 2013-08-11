#pragma once

#include <ydwe/exception/exception.h>
#include <string>
#include <vector>

_BASE_BEGIN 
namespace util {

	class buffer
	{
	public:
		typedef std::vector<char>           buffer_type;
		typedef buffer_type::iterator       iterator;
		typedef buffer_type::const_iterator const_iterator;
		typedef buffer_type::value_type     value_type;

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
	};

	class buffer_reader
	{
		typedef buffer BufferT;
	public:
		enum status
		{
			normal = 0,
			stream_eof,
		};

		typedef BufferT::buffer_type    buffer_type;
		typedef BufferT::iterator       iterator;
		typedef BufferT::const_iterator const_iterator;
		typedef BufferT::value_type     value_type;

		buffer_reader(const BufferT& b)
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

		value_type get(status& ec)
		{
			if (!(cur_ != end_))
			{
				ec = stream_eof;
				return 0;
			}

			value_type c = *cur_;
			++cur_;
			return static_cast<value_type>(c);
		}

		const value_type* reads_ptr(size_t n)
		{
			if (end_ - cur_ <  static_cast<buffer_type::difference_type>(n))
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

		const value_type* reads_ptr(size_t n, status& ec)
		{
			if (end_ - cur_ <  static_cast<buffer_type::difference_type>(n))
			{
				ec = stream_eof;
				return nullptr;
			}

			const value_type* ret = &*cur_;
			cur_ += n;

			return ret;
		}

		template <class _Ty>
		const _Ty* read_ptr(status& ec)
		{
			return reinterpret_cast<const _Ty*>(reads_ptr(sizeof(_Ty), ec));
		}

		template <class _Ty>
		_Ty read(status& ec)
		{
			const _Ty* p = read_ptr<_Ty>(ec);
			return p ? *p : _Ty();
		}

		template <>
		std::string read<std::string>(status& ec)
		{
			const_iterator start = cur_;
			value_type c = get(ec);
			while (ec != stream_eof && c != '\0') { c = get(ec); }
			return std::move(std::string(start, cur_));
		}

		template <>
		std::string read<std::string>()
		{
			status ec = normal;
			return std::move(read<std::string>(ec));
		}

		void seek(size_t offset, std::ios::seek_dir dir)
		{
			switch (dir)
			{
			case  std::ios::beg:
				{
					if (end_ - beg_ < static_cast<buffer_type::difference_type>(offset))
					{
						throw exception("buffer overflow.");
					}
					cur_ = beg_ + offset;
				}
				break;
			case  std::ios::cur:
				{
					if (end_ - cur_ < static_cast<buffer_type::difference_type>(offset))
					{
						throw exception("buffer overflow.");
					}
					cur_ = cur_ + offset;
				}
				break;
			case  std::ios::end:
				{
					if (end_ - beg_ < static_cast<buffer_type::difference_type>(offset))
					{
						throw exception("buffer overflow.");
					}
					cur_ = beg_ + (end_ - beg_ - offset);
				}
				break;
			}
		}

	private:
		const_iterator beg_;
		const_iterator end_;
		const_iterator cur_;
	};

}
_BASE_END
