
#include <slk/reader/WtsReader.hpp>
#include <slk/reader/CommonReader.hpp>
#include <slk/utility/sequence.h>
#include <slk/utility/convert.h>

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

	void WtsReader::Read(base::buffer_reader& reader, WtsTable& table)
	{
		WST_READER_STATE::STATE state = WST_READER_STATE::STATE_HEADER;
		uint32_t key = 0;
		std::string value;

		reader::utility::remove_bom(reader);
		reader::utility::each_line(reader, [&](std::string_view& line)
		{
			switch (state)
			{
			case WST_READER_STATE::STATE_HEADER:
				{
					trim_left(line);
					if (line.substr(0, 6) == "STRING")
					{
						line.remove_prefix(6);
						trim(line);
						key = Str2UInt(line);
						state = WST_READER_STATE::STATE_BEGIN;
					}
				}
				break;
			case WST_READER_STATE::STATE_BEGIN:
				{
					trim_left(line);
					if (!line.empty() && '{' == line[0])
					{
						state = WST_READER_STATE::STATE_BODY;
					}
				}
				break;
			case WST_READER_STATE::STATE_BODY:
				{
					std::string_view new_line = trim_left_copy(line);
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
