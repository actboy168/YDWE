#pragma once

#include <slk/table/SlkTable.hpp>
#include <slk/table/ObjTable.hpp>
#include <slk/table/MetaTable.hpp>
#include <slk/Converter.hpp>

namespace slk
{
	template <typename _Ty>
	void SingleAppend(_Ty& SrcT, _Ty const& MoreT)
	{
		foreach(auto const& It, MoreT)
		{
			SrcT[It.first] = It.second;
		}	
	}

	template <typename _Ty>
	void TableAppend(_Ty& SrcT, _Ty const& MoreT)
	{
		foreach(auto const& It, MoreT)
		{
			SingleAppend(SrcT[It.first], It.second);			
		}
	}

	template <typename _Ty>
	void TableAppendIfExist(_Ty& SrcT, _Ty const& MoreT)	
	{
		foreach(auto const& It, MoreT)
		{
			auto SrcTIt = SrcT.find(It.first);

			if (SrcTIt != SrcT.end())
			{
				SingleAppend(SrcTIt->second, It.second);	
			}		
		}
	}

	template <OBJECT_PARSER_OPTION Option>
	void ObjTableToSlkTable(SlkTable& slkobjTable, ObjTable<Option> const& objTable, SlkTable const& baseTable, MetaTable const& metaTable, Converter& converter);

	template <OBJECT_PARSER_OPTION Option>
	void ObjTableToSlkTable(SlkTable& slkobjTable, ObjTable<Option> const& objTable, MetaTable const& metaTable, Converter& converter);

	template <OBJECT_PARSER_OPTION Option>
	void SlkTableToObjTable(ObjTable<Option>& objTable, SlkTable const& slkTable, MetaTable const& metaTable);
}
