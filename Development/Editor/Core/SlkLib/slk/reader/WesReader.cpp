
#include <slk/reader/WesReader.hpp>
#include <slk/reader/CommonReader.hpp>

namespace slk
{
	void WesReader::Read(base::buffer_reader& reader, WesTable& table)
	{
		bool is_WorldEditStrings = false;
		reader::utility::remove_bom(reader);
		reader::utility::each_line(reader, [&](std::string_view& line)
		{
			size_t pos = line.find("//");
			if (pos != std::string_view::npos)
			{
				line.remove_prefix(pos);
			}

			trim_left(line);

			if ((line.size() >= 2) && (line[0] == '['))
			{
				size_t n = line.find_first_of(']');
				if (n != std::string_view::npos)
				{
					is_WorldEditStrings = "WorldEditStrings" == trim_copy(line.begin() + 1, line.begin() + n);
				}
			}
			else
			{
				if (is_WorldEditStrings)
				{
					size_t n = line.find_first_of('=');
					if (n != std::string_view::npos)
					{
						std::string_view key = trim_copy(line.begin(), line.begin() + n);
						std::string_view val = trim_copy(line.begin() + n + 1, line.end());
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
