#include <slk/reader/MetaReader.hpp>
#include <slk/reader/SlkReader.hpp>
#include <base/util/buffer.h>
#include <slk/table/MetaTable.hpp>
#include <cassert>

namespace slk
{
	namespace detail
	{
		VariableData::Type ConvertVariableDataType(std::string const& attrType)
		{
			VariableData::Type slkType;

			if (attrType == "unreal")
				slkType = VariableData::OBJTYPE_UNREAL;
			else if (attrType == "real")
				slkType = VariableData::OBJTYPE_REAL;
			else if (attrType == "int"
				|| attrType == "bool"
				|| attrType == "spellDetail"
				|| attrType == "morphFlags"
				|| attrType == "silenceFlags"
				|| attrType == "stackFlags"
				|| attrType == "pickFlags"
				|| attrType == "defenseTypeInt"
				|| attrType == "attackBits"
				|| attrType == "interactionFlags"
				|| attrType == "detectionType"
				|| attrType == "versionFlags"
				|| attrType == "channelFlags"
				|| attrType == "channelType"
				|| attrType == "deathType"
				|| attrType == "fullFlags"
				|| attrType == "teamColor")
				slkType = VariableData::OBJTYPE_INTEGER;
			else
				slkType = VariableData::OBJTYPE_STRING;

			return slkType;
		}

		std::string SlkSingleGetValue(SlkSingle const& slkSingle, std::string const& key)
		{
			auto It = slkSingle.find(key);
			if (It == slkSingle.end())
			{
				return std::move(std::string(""));
			}

			return It->second.to_string();
		}

		MetaSingle SlkSingleToMetaSingle(SlkSingle const& slkSingle)
		{
			MetaSingle val;

			// type
			auto TypeIt = slkSingle.find("type");
			if (TypeIt == slkSingle.end())
			{
				assert(false);
				throw base::exception("meta slk file miss 'type'.");
			}
			std::string const& TypeStr = TypeIt->second.to_string();
			val.type = ConvertVariableDataType(TypeStr);

			// index
			auto IndexIt = slkSingle.find("index");
			if (IndexIt == slkSingle.end())
			{
				assert(false);
				throw base::exception("meta slk file miss 'index'.");
			}
			val.index = std::stol(IndexIt->second.to_string());

			// stringExt
			auto StringExtIt = slkSingle.find("stringExt");
			if (StringExtIt == slkSingle.end())
			{
				assert(false);
				throw base::exception("meta slk file miss 'stringExt'.");
			}
			val.stringExt = std::stol(StringExtIt->second.to_string());

			// field
			auto FieldIt = slkSingle.find("field");
			if (FieldIt == slkSingle.end())
			{
				assert(false);
				throw base::exception("meta slk file miss 'field'.");
			}
			if (FieldIt->second.to_string() == "Data")
			{
				auto DataIt = slkSingle.find("data");
				if (DataIt == slkSingle.end())
				{
					assert(false);
					throw base::exception("meta slk file miss 'data'.");
				}

				std::string const& Data = DataIt->second.to_string();
				if (Data.size() <= 0)
				{
					assert(false);
					throw base::exception("meta slk file miss 'data'.");
				}

				val.field = "Data"+ std::string(1, static_cast<char>('A'+Data[0]-'0'-1));
			}
			else
			{
				val.field = FieldIt->second.to_string();
			}

			// slk
			val.slk = SlkSingleGetValue(slkSingle, "slk");

			return std::move(val);
		}
	}

	void MetaReader::Read(base::buffer_reader& reader, MetaTable& table, bool /*create_if_not_exists*/)
	{
		SlkTable slkTable;
		SlkReader::Read(reader, slkTable, true);

		foreach(auto const& It, slkTable)
		{
			table.insert(std::move(std::make_pair(It.first, detail::SlkSingleToMetaSingle(It.second)))).second;
		}
	}
}
