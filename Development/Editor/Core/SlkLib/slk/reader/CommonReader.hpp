#pragma once

#include <slk/utility/sequence.h>
#include <slk/utility/buffer.h>
#include <functional>
#include <base/util/string_view.h>

namespace slk { namespace reader { namespace utility {
	void remove_bom(buffer_reader& reader);
	void each_line(buffer_reader& reader, std::function<void(std::string_view&)> callback);

	template <class TableT>
	void ini_read(buffer_reader& reader, TableT& table)
	{
		typename TableT::mapped_type* object = nullptr;
		remove_bom(reader);
		each_line(reader, [&](std::string_view& line)
		{
			size_t pos = line.find("//");
			if (pos != std::string_view::npos)
			{
				line.remove_prefix(pos);
			}

			trim_left(line, ctype::is_space());

			if ((line.size() >= 2) && (line[0] == '['))
			{
				auto ItBeg = find_begin(line, char_equal(']'));
				if (ItBeg != line.end())
				{
					object = &table[trim_copy(line.begin()+1, ItBeg).to_string()];
				}
			}
			else
			{
				if (object)
				{
					auto ItBeg = find_begin(line, char_equal('='));

					if (ItBeg != line.end())
					{
						std::string_view key = trim_copy(line.begin(), ItBeg);
						std::string_view val = trim_copy(ItBeg+1, line.end());
						if (!val.empty() && !key.empty())
						{
							(*object)[key.to_string()] = val.to_string();
						}
					}
				}
			}
		});
	}
}}}

