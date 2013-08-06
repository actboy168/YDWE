#pragma once

#include "port/config.h"
#include <ydwe/exception/exception.h>
#include <string>
#include <vector>

namespace slk
{
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

	private:
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

		template <class _Ty>
		_Ty* read_ptr()
		{
			if (end_ - cur_ < static_cast<buffer_type::difference_type>(sizeof(_Ty)))
			{
				throw ydwe::exception("buffer overflow.");
			}

			_Ty* pRet = reinterpret_cast<_Ty*>(&*cur_);
			cur_ += sizeof(_Ty);

			return pRet;
		}

		template <class _Ty>
		_Ty read()
		{
			return * read_ptr<_Ty>();
		}

		template <class _Ty>
		_Ty* read_ptr(status& ec)
		{
			if (end_ - cur_ <  static_cast<buffer_type::difference_type>(sizeof(_Ty)))
			{
				ec = stream_eof;
				return nullptr;
			}

			_Ty* pRet = reinterpret_cast<_Ty*>(&*cur_);
			cur_ += sizeof(_Ty);

			return pRet;
		}

		template <class _Ty>
		_Ty read(status& ec)
		{
			_Ty* p = read_ptr<_Ty>(ec);
			return p ? *p : _Ty();
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
}
