#pragma once


//+-----------------------------------------------------------------------------
//| Included files
//+-----------------------------------------------------------------------------
#include "Definitions.h"
#include <string>

namespace IMAGE 
{

//+-----------------------------------------------------------------------------
//| Buffer class
//+-----------------------------------------------------------------------------
class BUFFER
{
public:
	typedef std::string                 buffer_type;
	typedef buffer_type::iterator       iterator;
	typedef buffer_type::const_iterator const_iterator;

	BUFFER();
	BUFFER(const std::string& CopyString);
	~BUFFER();

	template <class InputIterator>
	BUFFER(InputIterator begin, InputIterator end)
		: buffer_(begin, end)
	{ }

	template <class InputIterator>
	void Assign(InputIterator begin, InputIterator end)
	{
		buffer_.assign(begin, end);
	}

	void Resize(size_t NewSize);
	unsigned char*       GetData(size_t Index = 0);
	const unsigned char* GetData(size_t Index = 0) const;
	size_t               GetSize() const;
	char& operator [](size_t Index);
	std::string const& Buf() const;

	iterator begin()
	{
		return buffer_.begin();
	}

	const_iterator begin() const
	{
		return buffer_.begin();
	}

	iterator end()
	{
		return buffer_.end();
	}

	const_iterator end() const
	{
		return buffer_.end();
	}

protected:
	buffer_type buffer_;
};

}
