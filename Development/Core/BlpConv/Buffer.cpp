//+-----------------------------------------------------------------------------
//| Included files
//+-----------------------------------------------------------------------------
#include "Buffer.h"


namespace IMAGE 
{

//+-----------------------------------------------------------------------------
//| Constructor
//+-----------------------------------------------------------------------------
BUFFER::BUFFER()
	: buffer_()
{
}

BUFFER::BUFFER(const std::string& CopyString)
	: buffer_(CopyString)
{
}


//+-----------------------------------------------------------------------------
//| Destructor
//+-----------------------------------------------------------------------------
BUFFER::~BUFFER()
{
}


//+-----------------------------------------------------------------------------
//| Resizes the buffer
//+-----------------------------------------------------------------------------
void BUFFER::Resize(size_t NewSize)
{
	buffer_.resize(NewSize);
}


//+-----------------------------------------------------------------------------
//| Returns the data from the buffer
//+-----------------------------------------------------------------------------
unsigned char* BUFFER::GetData(size_t Index)
{
	return const_cast<unsigned char*>((const unsigned char*)buffer_.data()) + Index;
}

const unsigned char* BUFFER::GetData(size_t Index) const
{
	return (const unsigned char*)buffer_.data() + Index;
}

//+-----------------------------------------------------------------------------
//| Returns the buffer size
//+-----------------------------------------------------------------------------
size_t BUFFER::GetSize() const
{
	return buffer_.size();
}


//+-----------------------------------------------------------------------------
//| Reference a byte in the buffer
//+-----------------------------------------------------------------------------
char& BUFFER::operator [](size_t Index)
{
	return buffer_[Index];
}

std::string const& BUFFER::Buf() const 
{
	return buffer_;
};

}
