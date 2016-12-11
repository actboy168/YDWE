#include <slk/reader/ObjReader.hpp>
#include <cassert>

namespace slk
{
	template <>
	void ObjReader::ReadModData<ObjectWithOptinal>(base::buffer_reader& reader, ObjSingle<ObjectWithOptinal>& obj)
	{
		uint32_t nModCount = reader.read<uint32_t>();
		for (uint32_t i = 0; i < nModCount; ++i)
		{
			// Mod id
			object_id modid(reader.read<uint32_t>(), object_id::not_swap_t());
			assert (modid.vaild());

			// Type
			VariableData data;
			data.SetType(reader.read<VariableData::Type>());

			// Level/variation
			uint32_t level = reader.read<uint32_t>();

			// data indicator
			uint32_t dataIndicator = reader.read<uint32_t>();

			// Get data
			data.LoadData(reader);

			// CheckId
			reader.read_ptr<uint32_t>();

			// Add attribute
			Attribute<ObjectWithOptinal> attr;
			attr.SetId(modid);
			attr.SetLevel(level);
			attr.SetDataIndicator(dataIndicator);
			attr.SetData(std::move(data));

			obj.Insert(std::move(attr));
		}
	}

	template <>
	void ObjReader::ReadModData<ObjectWithoutOptinal>(base::buffer_reader& reader, ObjSingle<ObjectWithoutOptinal>& obj)
	{
		uint32_t nModCount = reader.read<uint32_t>();
		for (uint32_t i = 0; i < nModCount; ++i)
		{
			// Mod id
			object_id modid(reader.read<uint32_t>(), object_id::not_swap_t());

			// Type
			VariableData data;
			data.SetType(reader.read<VariableData::Type>());

			// Get data
			data.LoadData(reader);

			// CheckId
			reader.read_ptr<uint32_t>();

			// Add attribute
			Attribute<ObjectWithoutOptinal> attr;
			attr.SetId(modid);
			attr.SetData(std::move(data));

			obj.Insert(std::move(attr));
		}
	}
}
