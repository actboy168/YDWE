#pragma once

#include "Buffer.hpp"
#include "Util.hpp"
#include <functional>

namespace slk
{
	struct TextReader
	{
		static void RemoveBom(buffer_reader& reader)
		{
			buffer_reader::status ec = buffer_reader::normal;
			if ((0xEF == static_cast<uint8_t>(reader.get(ec)))
				&& (0xBB == static_cast<uint8_t>(reader.get(ec)))
				&& (0xBF == static_cast<uint8_t>(reader.get(ec))))
			{
			}
			else
			{
				reader.reset();
			}
		}

		template <class SequenceT>
		static void EachLine(buffer_reader& reader, std::function<void(SequenceT&)> callback)
		{
			buffer_reader::status ec = buffer_reader::normal;
			while (ec != buffer_reader::stream_eof)
			{
				SequenceT value;
				buffer_reader::value_type c = reader.get(ec);
				while (ec != buffer_reader::stream_eof && c != '\n') { value.push_back(c); c = reader.get(ec); }
				callback(value);
			}
		}
	};
}
