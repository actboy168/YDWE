#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <slk/table/Attribute.hpp>
#include <slk/table/SlkTable.hpp>
#include <slk/table/MetaTable.hpp>
#include <slk/Converter.hpp>

namespace slk
{
	template <class _Table, class _Value>
	bool TableGetValueById(_Table const& table, object_id const& id, _Value const** ppval)
	{
		if (!ppval)
		{
			return false;
		}

		auto const& It = table.find(id);
		if (It == table.end())
		{
			return false;
		}

		*ppval = &It->second;
		return true;
	}

	template <OBJECT_PARSER_OPTION Option>
	class AttributeMeta
	{
	public:
		AttributeMeta(Attribute<Option> const& that, MetaTable const& metaTable, Converter& converter);
		void               Set(SlkSingle& obj) const;
		MetaSingle const*  GetMeta() const;

	private:
		bool               Init(Attribute<Option> const& that, MetaTable const& metaTable, Converter& converter);
		std::string        ToSlkName(Attribute<Option> const& that) const;

	private:
		bool valid_;
		std::string name_;
		std::string value_;
		MetaSingle const* meta_;
	};

	template <OBJECT_PARSER_OPTION Option>
	AttributeMeta<Option>::AttributeMeta(Attribute<Option> const& that, MetaTable const& metaTable, Converter& converter)
	{ 
		valid_ = Init(that, metaTable, converter);
	}

	template <OBJECT_PARSER_OPTION Option>
	bool AttributeMeta<Option>::Init(Attribute<Option> const& that, MetaTable const& metaTable, Converter& converter)
	{
		if (!TableGetValueById(metaTable, that.GetId(), &meta_))
		{
			return false;
		}

		name_ = ToSlkName(that);
		VariableData const& data = that.GetData();
		if (data.GetType() != meta_->type)
			return false;

		value_ = data.ToString();

		if (meta_->stringExt)
		{
			value_ = converter.wts.Convert(value_);
		}

		return true;
	}

	template <OBJECT_PARSER_OPTION Option>
	void AttributeMeta<Option>::Set(SlkSingle& obj) const
	{
		if (valid_)
		{
			if (meta_->index == uint32_t(-1))
			{
				if (name_ == "auto" && meta_->slk == "UnitAbilities" && value_ == "")
				{
					obj[name_] = SlkValue("_", meta_->type == VariableData::OBJTYPE_STRING);
				}
				else
				{
					obj[name_] = SlkValue(value_, meta_->type == VariableData::OBJTYPE_STRING);
				}
				return ;
			}

			auto it = obj.find(name_);
			if (it == obj.end())
			{
				obj[name_] = SlkValue(value_, meta_->type == VariableData::OBJTYPE_STRING);
				return ;
			}

			SlkValue& val = it->second;
			std::vector<std::string> result;
			split(result, val.to_string(), char_equal(','));

			if (meta_->index >= result.size())
			{
				return ;
			}

			result[meta_->index] = value_;
			val.set(concat(result, ','));
		}
	}

	template <OBJECT_PARSER_OPTION Option>
	MetaSingle const*  AttributeMeta<Option>::GetMeta() const
	{
		if (!valid_) 
			return nullptr;

		return meta_;
	}
}
