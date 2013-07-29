
#include "WtsReader.hpp"
#include "TextReader.hpp"
#include "Sequence.hpp"

namespace slk
{
	namespace WST_READER_STATE
	{
		enum STATE
		{
			STATE_HEADER,
			STATE_BEGIN,
			STATE_BODY,
		};
	}

	void WtsReader::Read(buffer&& buf, WtsTable& table)
	{
		WST_READER_STATE::STATE state = WST_READER_STATE::STATE_HEADER;
		uint32_t key = 0;
		std::string value;

		TextReader::RemoveBom(buf);
		TextReader::EachLine<std::string>(buf, [&](std::string& line)
		{
			switch (state)
			{
			case WST_READER_STATE::STATE_HEADER:
				{
					trim_left(line, ctype::is_space());
					if (0 == line.compare(0, 6, "STRING"))
					{
						line.erase(0, 6);
						trim(line, ctype::is_space());
						key = Str2UInt(line);
						state = WST_READER_STATE::STATE_BEGIN;
					}
				}
				break;
			case WST_READER_STATE::STATE_BEGIN:
				{
					trim_left(line, ctype::is_space());
					if ('{' == line[0])
					{
						state = WST_READER_STATE::STATE_BODY;
					}
				}
				break;
			case WST_READER_STATE::STATE_BODY:
				{
					if ('}' != trim_left_copy(line, ctype::is_space())[0])
					{
						value.append(line);
					}
					else
					{
						while ((*value.rbegin() == '\n') || (*value.rbegin() == '\r'))
						{
							value.pop_back();
						}

						table[key] = value;
						value.clear();
						state = WST_READER_STATE::STATE_HEADER;
					}
				}
				break;
			default:
				break;
			}
		}
		);
	}
}
