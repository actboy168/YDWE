#include <slk/reader/CommonReader.hpp>
#include <slk/utility/buffer.h>
#include <functional>
#include <cstdint>

namespace slk { namespace reader { namespace utility {

	void remove_bom(buffer_reader& reader)
	{
		std::error_code ec;
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

	void each_line(buffer_reader& reader, std::function<void(boost::string_ref&)> callback)
	{
		std::error_code ec;
		while (ec)
		{
			const char* ptr = reader.read_ptr<char>(ec);
			size_t len = 0;
			for (const char* next = ptr
				; ec && *next != '\n'
				; next = reader.read_ptr<char>(ec), ++len)
				;

			if (len != 0)
			{
				boost::string_ref value(ptr, ptr+len);
				callback(value);
			}
		}
	}
}}}
