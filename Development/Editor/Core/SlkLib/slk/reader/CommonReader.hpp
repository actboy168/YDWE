#pragma once

#include <slk/utility/sequence.h>
#include <slk/utility/buffer.h>
#include <functional>
#include <ydwe/util/string_ref.h>

namespace slk { namespace reader { namespace utility {
	void remove_bom(buffer_reader& reader);
	void each_line(buffer_reader& reader, std::function<void(boost::string_ref&)> callback);

	template <class TableT>
	void ini_read(buffer_reader& reader, TableT& table)
	{
		typename TableT::mapped_type* object = nullptr;
		remove_bom(reader);
		each_line(reader, [&](boost::string_ref& line)
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
					object = &table[trim_copy<boost::string_ref>(line.begin()+1, ItBeg).to_string()];
				}
			}
			else
			{
				if (object)
				{
					auto ItBeg = find_begin(line, char_equal('='));

					if (ItBeg != line.end())
					{
						boost::string_ref key = trim_copy<boost::string_ref>(line.begin(), ItBeg);
						boost::string_ref val = trim_copy<boost::string_ref>(ItBeg+1, line.end());
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

