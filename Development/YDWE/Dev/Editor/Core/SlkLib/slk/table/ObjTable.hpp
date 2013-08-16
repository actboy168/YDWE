#pragma once

#include <slk/table/BaseTable.hpp>
#include <slk/table/Attribute.hpp>

namespace slk
{
	template <OBJECT_PARSER_OPTION Option>
	class ObjSingle
	{
	public:
		void Insert(Attribute<ObjectWithoutOptinal>&& attr)
		{
			auto it = _attributes.find(attr.GetId());
			if (it == _attributes.end())
			{
				_attributes.insert(std::make_pair(attr.GetId(), std::forward<Attribute<ObjectWithoutOptinal>>(attr)));
			}
			else
			{
				it->second = std::forward<Attribute<ObjectWithoutOptinal>>(attr);
			}
		}

		void Insert(Attribute<ObjectWithOptinal>&& attr)
		{
			uint32_t key = (((attr.GetDataIndicator() <<16) & 0xFFFF) | (attr.GetLevel() & 0xFFFF));

			auto it = _attributes.find(attr.GetId());
			if (it == _attributes.end())
			{
				AttributeCatalog catalog;
				catalog.insert(std::make_pair(key, std::forward<Attribute<ObjectWithOptinal>>(attr)));
				_attributes.insert(std::make_pair(attr.GetId(), std::move(catalog)));
			}
			else
			{
				it->second[key] = std::forward<Attribute<ObjectWithOptinal>>(attr);
			}
		}

		object_id GetId() const
		{
			return _id;
		}
		void SetId(const object_id& id)
		{
			this->_id = id;
		}

		object_id GetBaseId() const
		{
			return _baseid;
		}
		void SetBaseId(const object_id& id)
		{
			this->_baseid = id;
		}

		AttributeTable<Option> const& GetAttributes() const
		{
			return _attributes;
		}
	private:
		object_id _id;
		object_id _baseid;
		AttributeTable<Option> _attributes;
	};

	template <OBJECT_PARSER_OPTION Option>
	struct ObjTable : public HashTable<object_id, ObjSingle<Option>>::Type
	{
		const static OBJECT_PARSER_OPTION table_type = Option;
	};
}
