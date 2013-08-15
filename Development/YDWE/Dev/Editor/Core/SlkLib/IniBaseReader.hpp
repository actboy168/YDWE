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
			TextReader::EachLine(reader, [&](boost::string_ref& line)
			{
				size_t pos = line.find("//");
				if (pos != boost::string_ref::npos)
				{
					line.remove_prefix(pos);
				}

				trim_left(line, ctype::is_space());

				if ((line.size() >= 2) && (line[0] == '['))
				{
					auto ItBeg = find_begin(line, char_equal(']'));
					if (ItBeg != line.end())
					{
						object = &table[String<boost::string_ref>(line.begin()+1, ItBeg).to_string()];
					}
				}
				else
				{
					if (object)
					{
						auto ItBeg = find_begin(line, char_equal('='));

						if (ItBeg != line.end())
						{
							boost::string_ref key = String<boost::string_ref>(line.begin(), ItBeg);
							boost::string_ref val = String<boost::string_ref>(ItBeg+1, line.end());
							if (!val.empty() && !key.empty())
							{
								(*object)[key.to_string()] = val.to_string();
							}
						}
					}
				}
			});
		}
	}
}
