#pragma once

#include <slk/utility/sequence.h>
#include <base/util/buffer.h>
#include <functional>
#include <base/util/string_view.h>

namespace slk { namespace reader { namespace utility {
	void remove_bom(base::util::buffer_reader& reader);
	void each_line(base::util::buffer_reader& reader, std::function<void(std::string_view&)> callback);

	template <class TableT>
	void ini_read(base::util::buffer_reader& reader, TableT& table)
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

			trim_left(line);

			if ((line.size() >= 2) && (line[0] == '['))
			{
				size_t n = line.find_first_of(']');
				if (n != std::string_view::npos)
				{
					object = &table[trim_copy(line.begin() + 1, line.begin() + n).to_string()];
				}
			}
			else
			{
				if (object)
				{
					size_t n = line.find_first_of('=');
					if (n != std::string_view::npos)
					{
						std::string_view key = trim_copy(line.begin(), line.begin() + n);
						std::string_view val = trim_copy(line.begin() + n + 1, line.end());
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

