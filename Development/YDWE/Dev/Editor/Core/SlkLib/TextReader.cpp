#include "TextReader.hpp"
#include <slk/utility/buffer.h>
#include <functional>
#include <cstdint>

namespace slk
{
	void TextReader::RemoveBom(buffer_reader& reader)
	{
		buffer_reader::status ec = buffer_reader::normal;
		if ((0xEF == reader.read<uint8_t>(ec))
			&& (0xBB == reader.read<uint8_t>(ec))
			&& (0xBF == reader.read<uint8_t>(ec)))
		{
		}
		else
		{
			reader.reset();
		}
	}

	void TextReader::EachLine(buffer_reader& reader, std::function<void(boost::string_ref&)> callback)
	{
		buffer_reader::status ec = buffer_reader::normal;
		while (ec != buffer_reader::stream_eof)
		{
			const char* ptr = reader.read_ptr<char>(ec);
			size_t len = 0;
			for (const char* next = ptr
				; ec != buffer_reader::stream_eof && *next != '\n'
				; next = reader.read_ptr<char>(ec), ++len)
				;

			if (len != 0)
			{
				boost::string_ref value(ptr, ptr+len);
				callback(value);
			}
		}
	}
}
