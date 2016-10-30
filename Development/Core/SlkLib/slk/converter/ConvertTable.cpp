
#include <slk/converter/ConvertTable.hpp>
#include <slk/converter/AttributeMeta.hpp>
#include <slk/utility/sequence.h>	
#include <base/util/string_algorithm.h>
#include <algorithm>

namespace slk
{
	namespace detail
	{
		template <typename ResultT, typename SequenceT>
		inline ResultT& SplitObjectId(ResultT& Result, SequenceT& Input)
		{
			typedef typename SequenceT::value_type CharT;
			split(Result, Input, char_equal(static_cast<CharT>(',')));

			typedef typename ResultT::iterator ForwardIteratorT;
			ForwardIteratorT InBegin = Result.begin();
			ForwardIteratorT InEnd = Result.end();

			for (ForwardIteratorT It = InBegin; It != InEnd; ++It)
			{
				base::algorithm::trim(*It);
			}
			std::remove_if(Result.begin(), Result.end(), [](typename ResultT::reference& It){ return It.size() != 4;});

			return Result;
		}
	}

	template <>
	void ObjTableToSlkTable<ObjectWithoutOptinal>(SlkTable& slkobjTable, ObjTable<ObjectWithoutOptinal> const& objTable, SlkTable const& baseTable, MetaTable const& metaTable, Converter& converter)
	{
		foreach(auto& it, objTable)
		{
			SlkSingle& slkObj = slkobjTable[it.first];
			ObjSingle<ObjectWithoutOptinal> const& objObj = it.second;
			AttributeTable<ObjectWithoutOptinal> const& attrTable = objObj.GetAttributes();

			auto const& baseObjIt = baseTable.find(objObj.GetBaseId());
			if (baseObjIt != baseTable.end())
			{
				SingleAppend(slkObj, baseObjIt->second);
			}

			slkObj["code"] = objObj.GetBaseId().to_string();

			foreach(auto& it, attrTable)
			{
				AttributeMeta<ObjectWithoutOptinal>(it.second, metaTable, converter).Set(slkObj);
			}
		}
	}

	template <>
	void ObjTableToSlkTable<ObjectWithOptinal>(SlkTable& slkobjTable, ObjTable<ObjectWithOptinal> const& objTable, SlkTable const& baseTable, MetaTable const& metaTable, Converter& converter)
	{
		foreach(auto& it, objTable)
		{
			SlkSingle& slkObj = slkobjTable[it.first];
			ObjSingle<ObjectWithOptinal> const& objObj = it.second;
			AttributeTable<ObjectWithOptinal> const& attrTable = objObj.GetAttributes();

			auto const& baseObjIt = baseTable.find(objObj.GetBaseId());
			if (baseObjIt != baseTable.end())
			{
				SingleAppend(slkObj, baseObjIt->second);
			}

			slkObj["code"] = objObj.GetBaseId().to_string();

			foreach(auto& it, attrTable)
			{
				//ObjectId attrId = it.first;
				AttributeCatalog const& attrCatalog = it.second;

				foreach(auto& it, attrCatalog)
				{
					AttributeMeta<ObjectWithOptinal>(it.second, metaTable, converter).Set(slkObj);
				}
			}
		}
	}

	template <>
	void ObjTableToSlkTable(SlkTable& slkobjTable, ObjTable<ObjectWithoutOptinal> const& objTable, MetaTable const& metaTable, Converter& converter)
	{
		foreach(auto& it, objTable)
		{
			SlkSingle& slkObj = slkobjTable[it.first];
			ObjSingle<ObjectWithoutOptinal> const& objObj = it.second;
			AttributeTable<ObjectWithoutOptinal> const& attrTable = objObj.GetAttributes();

			slkObj["code"] = objObj.GetBaseId().to_string();

			foreach(auto& it, attrTable)
			{
				AttributeMeta<ObjectWithoutOptinal>(it.second, metaTable, converter).Set(slkObj);
			}
		}
	}

	template <>
	void ObjTableToSlkTable(SlkTable& slkobjTable, ObjTable<ObjectWithOptinal> const& objTable, MetaTable const& metaTable, Converter& converter)
	{
		foreach(auto& it, objTable)
		{
			SlkSingle& slkObj = slkobjTable[it.first];
			ObjSingle<ObjectWithOptinal> const& objObj = it.second;
			AttributeTable<ObjectWithOptinal> const& attrTable = objObj.GetAttributes();

			slkObj["code"] = objObj.GetBaseId().to_string();

			foreach(auto& it, attrTable)
			{
				//ObjectId attrId = it.first;
				AttributeCatalog const& attrCatalog = it.second;

				foreach(auto& it, attrCatalog)
				{
					AttributeMeta<ObjectWithOptinal>(it.second, metaTable, converter).Set(slkObj);
				}
			}
		}
	}

	template <>
	void SlkTableToObjTable(ObjTable<ObjectWithoutOptinal>& /*objTable*/, SlkTable const& /*slkTable*/, MetaTable const& /*metaTable*/)
	{
		// Fixed me
		return ;
	}

	template <>
	void SlkTableToObjTable(ObjTable<ObjectWithOptinal>& /*objTable*/, SlkTable const& /*slkTable*/, MetaTable const& /*metaTable*/)
	{
		// Fixed me
		return ;
	}
}
