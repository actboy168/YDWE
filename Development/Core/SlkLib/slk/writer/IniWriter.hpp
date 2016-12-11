#pragma once

#include <base/util/buffer.h>
#include <slk/table/IniTable.hpp>

namespace slk
{
	class IniWriter
	{
	public:
		template <typename SequenceT>
		static SequenceT Write(IniTable const& table)
		{
			std::string output;
			foreach (auto const& SectionIt, table)
			{
				std::string const& Section = SectionIt.first;
				output += "[" + Section + "]\n";
				foreach (auto const& KeyValueIt, SectionIt.second)
				{
					std::string const& Key   = KeyValueIt.first;
					std::string const& Value = KeyValueIt.second;
					output += Key + " = " + Value + "\n";
				}
			}

			return std::move(SequenceT(output));
		}
	};
}
