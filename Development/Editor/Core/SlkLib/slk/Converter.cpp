
#include <slk/Converter.hpp>
#include <slk/reader/WtsReader.hpp>
#include <slk/reader/WesReader.hpp>
#include <slk/InterfaceStorm.hpp>
#include <slk/utility/sequence.h>
#include <slk/utility/convert.h>
#include <boost/algorithm/string.hpp>

namespace slk
{

	WtsConverter::WtsConverter(InterfaceStorm& storm)
		: _Mybase(storm)
	{ }

	const std::string& WtsConverter::Convert(const std::string& str)
	{
		_Mybase::initialize("war3map.wts");

		if (0 == str.compare(0, _countof("TRIGSTR_") - 1, "TRIGSTR_"))
		{
			uint32_t index = Str2UInt(str.substr(_countof("TRIGSTR_") - 1));
			auto It = _Mybase::table_.find(index);
			if (It != _Mybase::table_.end())
			{
				return  It->second;
			}
		}

		return str;
	}

	WesConverter::WesConverter(InterfaceStorm& storm)
		: _Mybase(storm)
	{ }

	const std::string& WesConverter::Convert(const std::string& str)
	{
		_Mybase::initialize("UI\\WorldEditStrings.txt");

		if (0 == str.compare(0, _countof("WESTRING_") - 1, "WESTRING_"))
		{
			auto It = _Mybase::table_.find(str);
			if (It != _Mybase::table_.end())
			{
				return It->second;
			}
		}

		return str;
	}


	const std::string& DefaultValueConverter::Convert(const std::string& str) const
	{
		std::string tmp = boost::algorithm::trim_copy(str);
		if (tmp == "-")
		{
			static std::string s_dummy_zero = "0";
			return s_dummy_zero;
		}
		else if (tmp == "_")
		{
			static std::string s_dummy_empty = "";
			return s_dummy_empty;
		}

		return str;
	}


	Converter::Converter(InterfaceStorm& storm)
		: wts(storm)
		, wes(storm)
	{ }
}
