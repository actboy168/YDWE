#include "ObjReader.hpp"

namespace slk
{
	template <>
	void ObjReader::ReadModData<ObjectWithOptinal>(buffer& buf, ObjSingle<ObjectWithOptinal>& obj)
	{
		uint32_t nModCount = buf.read<uint32_t>();
		for (uint32_t i = 0; i < nModCount; ++i)
		{
			// Mod id
			ObjectId modid(buf.read<uint32_t>(), ObjectId::not_swap_t());
			assert (modid.vaild());

			// Type
			VariableData data;
			data.SetType(buf.read<VariableData::Type>());

			// Level/variation
			uint32_t level = buf.read<uint32_t>();

			// data indicator
			uint32_t dataIndicator = buf.read<uint32_t>();

			// Get data
			data.LoadData(buf);

			// CheckId
			buf.read_ptr<uint32_t>();

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
	void ObjReader::ReadModData<ObjectWithoutOptinal>(buffer& buf, ObjSingle<ObjectWithoutOptinal>& obj)
	{
		uint32_t nModCount = buf.read<uint32_t>();
		for (uint32_t i = 0; i < nModCount; ++i)
		{
			// Mod id
			ObjectId modid(buf.read<uint32_t>(), ObjectId::not_swap_t());

			// Type
			VariableData data;
			data.SetType(buf.read<VariableData::Type>());

			// Get data
			data.LoadData(buf);

			// CheckId
			buf.read_ptr<uint32_t>();

			// Add attribute
			Attribute<ObjectWithoutOptinal> attr;
			attr.SetId(modid);
			attr.SetData(std::move(data));

			obj.Insert(std::move(attr));
		}
	}
}
