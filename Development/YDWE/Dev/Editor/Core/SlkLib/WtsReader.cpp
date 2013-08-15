
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

	void WtsReader::Read(buffer_reader& reader, WtsTable& table)
	{
		WST_READER_STATE::STATE state = WST_READER_STATE::STATE_HEADER;
		uint32_t key = 0;
		std::string value;

		TextReader::RemoveBom(reader);
		TextReader::EachLine(reader, [&](boost::string_ref& line)
		{
			switch (state)
			{
			case WST_READER_STATE::STATE_HEADER:
				{
					trim_left(line, ctype::is_space());
					if (line.substr(0, 6) == "STRING")
					{
						line.remove_prefix(6);
						trim(line, ctype::is_space());
						key = Str2UInt(line);
						state = WST_READER_STATE::STATE_BEGIN;
					}
				}
				break;
			case WST_READER_STATE::STATE_BEGIN:
				{
					trim_left(line, ctype::is_space());
					if (!line.empty() && '{' == line[0])
					{
						state = WST_READER_STATE::STATE_BODY;
					}
				}
				break;
			case WST_READER_STATE::STATE_BODY:
				{
					boost::string_ref new_line = trim_left_copy(line, ctype::is_space());
					if (new_line.empty() || ('}' != new_line[0]))
					{
						value.append(line.begin(), line.end());
					}
					else
					{
						while ((!value.empty()) && ((*value.rbegin() == '\n') || (*value.rbegin() == '\r')))
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
