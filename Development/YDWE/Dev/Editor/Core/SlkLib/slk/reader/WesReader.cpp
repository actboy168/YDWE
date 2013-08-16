
#include <slk/reader/WesReader.hpp>
#include <slk/reader/TextReader.hpp>
#include <slk/reader/IniBaseReader.hpp>

namespace slk
{
	void WesReader::Read(buffer_reader& reader, WesTable& table)
	{
		bool is_WorldEditStrings = false;
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
					is_WorldEditStrings = "WorldEditStrings" == IniBaseReader::String<boost::string_ref>(line.begin()+1, ItBeg);
				}
			}
			else
			{
				if (is_WorldEditStrings)
				{
					auto ItBeg = find_begin(line, char_equal('='));

					if (ItBeg != line.end())
					{
						boost::string_ref key = IniBaseReader::String<boost::string_ref>(line.begin(), ItBeg);
						boost::string_ref val = IniBaseReader::String<boost::string_ref>(ItBeg+1, line.end());
						if (!val.empty() && !key.empty())
						{
							table[key.to_string()] = val.to_string();
						}
					}
				}
			}
		});
	};
}
