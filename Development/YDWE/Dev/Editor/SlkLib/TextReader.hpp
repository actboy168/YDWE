#pragma once

#include "Buffer.hpp"
#include "Util.hpp"
#include <functional>

namespace slk
{
	struct TextReader
	{
		static void RemoveBom(buffer& buf)
		{
			buffer::status ec = buffer::normal;
			if ((0xEF == static_cast<uint8_t>(buf.get(ec)))
				&& (0xBB == static_cast<uint8_t>(buf.get(ec)))
				&& (0xBF == static_cast<uint8_t>(buf.get(ec))))
			{
			}
			else
			{
				buf.reset();
			}
		}

		template <class SequenceT>
		static void EachLine(buffer& buf, std::function<void(SequenceT&)> callback)
		{
			buffer::status ec = buffer::normal;
			while (ec != buffer::stream_eof)
			{
				SequenceT value;
				buffer::value_type c = buf.get(ec);
				while (ec != buffer::stream_eof && c != '\n') { value.push_back(c); c = buf.get(ec); }
				callback(value);
			}
		}
	};
}
