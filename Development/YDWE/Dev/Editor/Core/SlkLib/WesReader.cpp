
#include "WesReader.hpp"
#include "TextReader.hpp"
#include "IniBaseReader.hpp"

namespace slk
{
	void WesReader::Read(buffer&& buf, WesTable& table)
	{
		bool is_WorldEditStrings = false;
		TextReader::RemoveBom(buf);
		TextReader::EachLine<std::string>(buf, [&](std::string& line)
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
					is_WorldEditStrings = "WorldEditStrings" == IniBaseReader::String<std::string>(line.begin()+1, ItBeg);
				}
			}
			else
			{
				if (is_WorldEditStrings)
				{
					auto ItBeg = find_begin(line, char_equal('='));

					if (ItBeg != line.end())
					{
						std::string key = IniBaseReader::String<std::string>(line.begin(), ItBeg);
						std::string val = IniBaseReader::String<std::string>(ItBeg+1, line.end());
						if (!val.empty() && !key.empty())
						{
							table[key] = val;
						}
					}
				}
			}
		});
	};
}
