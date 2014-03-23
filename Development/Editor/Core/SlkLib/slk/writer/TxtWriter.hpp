#pragma once

#include <base/util/buffer.h>
#include <slk/table/SlkTable.hpp>

namespace slk
{
	class TxtWriter
	{
	public:
		template <typename SequenceT>
		static SequenceT Write(SlkTable const& table)
		{
			std::string output;
			foreach (auto const& ObjectIt, table)
			{
				object_id const& Id = ObjectIt.first;
				SlkSingle const& Object = ObjectIt.second;

				output += "[" + Id.to_string() + "]\n";
				foreach (auto const& KeyValueIt, Object)
				{
					std::string const& Key   = KeyValueIt.first;
					SlkValue const&    Value = KeyValueIt.second;
					output += Key + "=" + Value.to_string() + "\n";
				}
			}

			return std::move(SequenceT(output));
		}
	};
}
