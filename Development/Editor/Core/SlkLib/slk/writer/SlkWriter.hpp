#pragma once

#include <base/util/buffer.h>
#include <slk/table/SlkTable.hpp>

namespace slk
{
	struct SlkWriter
	{
		template <typename SequenceT>
		static SequenceT Write(SlkTable const& table, std::string const& tag = "ID")
		{
			std::string head;
			std::string body;
			std::map<std::string, size_t> TagList;
			size_t cur_x, cur_y = 2;

			foreach (auto const& ObjectIt, table)
			{
				object_id const& Id = ObjectIt.first;
				SlkSingle const& Object = ObjectIt.second;

				body += "C;X1;Y" + Int2Str(cur_y++) + ";K\"" + Id.to_string() + "\"\n";

				foreach (auto const& KeyValueIt, Object)
				{
					std::string const& Key   = KeyValueIt.first;
					SlkValue const&    Value = KeyValueIt.second;

					auto const& It = TagList.find(Key);
					if (It == TagList.end())
					{
						cur_x = TagList.size() + 2;
						TagList[Key] = cur_x;
					}
					else
					{
						cur_x = It->second;
					}

					body += "C;X" + Int2Str(cur_x) + ";K";
					if (Value.is_str())
					{
						body += "\"";
						body += Value.to_string();
						body += "\"";
					}
					else
					{
						body += Value.to_string();
					}
					body += "\n";
				}
			}

			std::vector<std::string> TagArray(TagList.size());
			foreach (auto const& It, TagList)
			{
				TagArray[It.second-2] = It.first;
			}

			head += "ID;PWXL;N;E\n";
			head += "B;X" + Int2Str(TagList.size()+1) + ";Y" + Int2Str(cur_y-1) + ";D0\n";
			head += "C;X1;Y1;K\"" + tag + "\"\n";
			cur_x = 2;
			foreach (auto const& It, TagArray)
			{
				head += "C;X" + Int2Str(cur_x++) + ";K\"" + It + "\"\n";
			}

			return std::move(SequenceT(head+body+"E\n"));
		}
	};
}
