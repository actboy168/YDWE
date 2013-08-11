#pragma once

#include <ydwe/exception/exception.h>
#include <string>
#include <vector>

_BASE_BEGIN namespace util {

	class buffer
	{
	public:
		enum status
		{
			normal = 0,
			stream_eof,
		};
		typedef std::vector<char>           buffer_type;
		typedef buffer_type::iterator       iterator;
		typedef buffer_type::const_iterator const_iterator;
		typedef buffer_type::value_type     value_type;

		static_assert(sizeof(value_type) == 1, "buffer::value_type's size must be 1.");

	protected:
		buffer_type buf_;
		iterator    cur_;
		iterator    end_;

	public:
		buffer()
			: buf_()
			, cur_(buf_.begin())
			, end_(buf_.end())
		{ }

		buffer(buffer&& source)
			: buf_(std::move(source.buf_))
			, cur_(buf_.begin())
			, end_(buf_.end())
		{ }

		template <class Source>
		buffer(Source const& source)
			: buf_(source.begin(), source.end())
			, cur_(buf_.begin())
			, end_(buf_.end())
		{ }

		template <class InputIterator>
		buffer(InputIterator begin, InputIterator end)
			: buf_(begin, end)
			, cur_(buf_.begin())
			, end_(buf_.end())
		{ }

		template <class Elem>
		buffer(Elem const* from, size_t length)
			: buf_(static_cast<value_type const*>(from), static_cast<value_type const*>(from)+length)
			, cur_(buf_.begin())
			, end_(buf_.end())
		{ }

		~buffer()
		{ }

		buffer& operator=(const buffer& source)
		{
			if (this != &source)
			{
				buf_ = source.buf_;
				cur_ = buf_.begin();
				end_ = buf_.end();
			}

			return *this;
		}

		buffer& operator=(buffer&& source)
		{
			if (this != &source)
			{
				buf_ = std::move(source.buf_);
				cur_ = buf_.begin();
				end_ = buf_.end();
			}

			return *this;
		}

		void reset()
		{
			cur_ = buf_.begin();
			end_ = buf_.end();
		}

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

		value_type* reads_ptr(size_t n)
		{
			if (end_ - cur_ <  static_cast<buffer_type::difference_type>(n))
			{
				throw exception("buffer overflow.");
			}

			value_type* ret = &*cur_;
			cur_ += n;

			return ret;
		}

		template <class _Ty>
		_Ty* read_ptr()
		{
			return reinterpret_cast<_Ty*>(reads_ptr(sizeof(_Ty)));
		}

		template <class _Ty>
		_Ty read()
		{
			return * read_ptr<_Ty>();
		}

		value_type* reads_ptr(size_t n, status& ec)
		{
			if (end_ - cur_ <  static_cast<buffer_type::difference_type>(n))
			{
				ec = stream_eof;
				return nullptr;
			}

			value_type* ret = &*cur_;
			cur_ += n;

			return ret;
		}

		template <class _Ty>
		_Ty* read_ptr(status& ec)
		{
			return reinterpret_cast<_Ty*>(reads_ptr(sizeof(_Ty), ec));
		}

		template <class _Ty>
		_Ty read(status& ec)
		{
			_Ty* p = read_ptr<_Ty>(ec);
			return p ? *p : _Ty();
		}

		void seek(size_t offset, std::ios::seek_dir dir)
		{
			switch (dir)
			{
			case  std::ios::beg:
				{
					if (end_ - buf_.begin() < static_cast<buffer_type::difference_type>(offset))
					{
						throw exception("buffer overflow.");
					}
					cur_ = buf_.begin() + offset;
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
					if (end_ - buf_.begin() < static_cast<buffer_type::difference_type>(offset))
					{
						throw exception("buffer overflow.");
					}
					cur_ = buf_.begin() + (end_ - buf_.begin() - offset);
				}
				break;
			}
		}
	};

	template <>
	inline std::string buffer::read<std::string>(buffer::status& ec)
	{
		iterator start = cur_;
		value_type c = get(ec);
		while (ec != stream_eof && c != '\0') { c = get(ec); }
		return std::move(std::string(start, cur_));
	}

	template <>
	inline std::string buffer::read<std::string>()
	{
		status ec = normal;
		return std::move(read<std::string>(ec));
	}
}}
