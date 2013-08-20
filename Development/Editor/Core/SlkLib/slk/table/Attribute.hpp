#pragma once

#include <slk/table/BaseTable.hpp>
#include <slk/utility/object_id.h>
#include <slk/table/VariableData.hpp>

namespace slk
{
	enum OBJECT_PARSER_OPTION
	{
		ObjectWithOptinal,
		ObjectWithoutOptinal,
	};

	template <OBJECT_PARSER_OPTION Option>
	class Attribute;

	template <>
	class Attribute<ObjectWithOptinal>
	{
	public:
		object_id GetId() const
		{
			return _id;
		}

		void SetId(const object_id& id)
		{
			this->_id = id;
		}

		uint32_t GetLevel() const
		{
			return _level;
		}

		void SetLevel(uint32_t level)
		{
			this->_level = level;
		}

		uint32_t GetDataIndicator() const
		{
			return _dataIndicator;
		}

		void SetDataIndicator(uint32_t dataIndicator)
		{
			this->_dataIndicator = dataIndicator;
		}

		VariableData const& GetData() const
		{
			return _data;
		}

		void SetData(VariableData&& data)
		{
			this->_data = std::move(data);
		}

	protected:
		object_id         _id;
		uint32_t         _level;
		uint32_t         _dataIndicator;
		VariableData     _data;
	};

	template <>
	class Attribute<ObjectWithoutOptinal>
	{
	public:
		object_id GetId() const
		{
			return _id;
		}

		void SetId(const object_id& id)
		{
			this->_id = id;
		}

		VariableData const& GetData() const
		{
			return _data;
		}

		void SetData(VariableData const& data)
		{
			this->_data = data;
		}	

	protected:
		object_id         _id;
		VariableData     _data;
	};

	typedef HashTable<uint32_t, Attribute<ObjectWithOptinal>>::Type AttributeCatalog;

	template <OBJECT_PARSER_OPTION Option>
	class AttributeTable;

	template <>
	class AttributeTable<ObjectWithOptinal> : public HashTable<object_id, AttributeCatalog>::Type { };

	template <>
	class AttributeTable<ObjectWithoutOptinal> : public HashTable<object_id, Attribute<ObjectWithoutOptinal>>::Type { };
}
