
#include <slk/Converter.hpp>
#include <slk/reader/WtsReader.hpp>
#include <slk/reader/WesReader.hpp>
#include <slk/InterfaceStorm.hpp>
#include <slk/utility/sequence.h>
#include <slk/utility/convert.h>
#include <base/util/string_algorithm.h>

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

	std::string_view WesConverter::Convert(const std::string_view& str)
	{
		_Mybase::initialize("UI\\WorldEditStrings.txt");

		if (0 == str.compare(0, _countof("WESTRING_") - 1, "WESTRING_"))
		{
			auto It = _Mybase::table_.find(str.to_string());
			if (It != _Mybase::table_.end())
			{
				return It->second;
			}
		}

		return str;
	}


	std::string_view DefaultValueConverter::Convert(const std::string_view& str) const
	{
		std::string_view tmp = str;
		base::algorithm::trim(tmp);
		if (tmp == "-")
		{
			static std::string_view s_dummy_zero = "0";
			return s_dummy_zero;
		}
		else if (tmp == "_")
		{
			static std::string_view s_dummy_empty = "";
			return s_dummy_empty;
		}

		return str;
	}


	Converter::Converter(InterfaceStorm& storm)
		: wts(storm)
		, wes(storm)
	{ }
}
