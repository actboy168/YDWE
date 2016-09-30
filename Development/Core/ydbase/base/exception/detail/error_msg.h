#include <base/config.h>
#include <Windows.h>

namespace base {

namespace exception_detail
{
	template <class CharT>
	class error_msg
	{
	public:
		error_msg()
			: buffer_(nullptr)
		{ }

		error_msg(size_t n)
			: buffer_(nullptr)
		{
			alloc(n);
		}

		error_msg(const error_msg& that)
			: buffer_(nullptr)
		{
			*this = that;
		}

		~error_msg()
		{
			free();
		}

		error_msg& operator =(const error_msg& that)
		{
			if (this != &that)
			{
				free();

				if (that.buffer_)
				{
					size_t size = ::LocalSize(that.buffer_);
					if (size > 0)
					{
						copy_str(that.buffer_, size / sizeof(CharT));
					}
				}
				else
				{
					buffer_ = that.buffer_;
				}
			}

			return *this;
		}

		void alloc(size_t n)
		{
			buffer_ = static_cast<CharT*>(::LocalAlloc(0, n * sizeof (CharT)));
		}

		void copy_str(const CharT* that_buffer, size_t that_size)
		{
			if (!that_buffer || !that_size)
			{
				free();
				return ;
			}

			alloc(that_size);

			if (buffer_)
			{
				memcpy(buffer_, that_buffer, that_size * sizeof (CharT));
			}
		}

		void free()
		{
			if (buffer_)
			{
				::LocalFree(reinterpret_cast<HLOCAL>(buffer_));
			}

			buffer_ = nullptr;
		}

		operator bool () const
		{
			return !!buffer_;
		}

		CharT* c_str()
		{
			return buffer_;
		}

		const CharT* c_str() const
		{
			return buffer_;
		}

		CharT** get()
		{
			return &buffer_;
		}

	private:
		CharT* buffer_;
	};
}

}
