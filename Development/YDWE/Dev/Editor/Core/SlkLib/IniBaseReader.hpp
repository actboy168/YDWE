#pragma once

#include "Sequence.hpp"
#include "Buffer.hpp"
#include "TextReader.hpp"

namespace slk
{
	namespace IniBaseReader
	{
		template <class SequenceT>
		static SequenceT String(typename SequenceT::const_iterator beg, typename SequenceT::const_iterator end)
		{
			return std::move(trim_copy<SequenceT>(beg, end, ctype::is_space()));
		}

		template <class TableT>
		void Read(buffer_reader& reader, TableT& table)
		{
			typename TableT::mapped_type* object = nullptr;
			TextReader::RemoveBom(reader);
			TextReader::EachLine(reader, [&](std::string& line)
			{
				size_t pos = line.find("//");
				if (pos != std::string::npos)
				{
					line.erase(pos);
				}

				trim_left(line, ctype::is_space());

				if ((line.size() >= 2) && (line[0] == '['))
				{
					auto ItBeg = find_begin(line, char_equal(']'));
					if (ItBeg != line.end())
					{
						object = &table[String<std::string>(line.begin()+1, ItBeg)];
					}
				}
				else
				{
					if (object)
					{
						auto ItBeg = find_begin(line, char_equal('='));

						if (ItBeg != line.end())
						{
							std::string key = String<std::string>(line.begin(), ItBeg);
							std::string val = String<std::string>(ItBeg+1, line.end());
							if (!val.empty() && !key.empty())
							{
								(*object)[key] = val;
							}
						}
					}
				}
			});
		}
	}
}
