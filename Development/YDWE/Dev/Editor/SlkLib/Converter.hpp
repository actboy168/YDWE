#pragma once

#include "WtsReader.hpp"
#include "WesReader.hpp"
#include "InterfaceStorm.hpp"
#include "Sequence.hpp"

namespace slk
{
	template <class Table, class Reader>
	class BaseConverter
	{
	public:
		BaseConverter(InterfaceStorm& storm)
			: storm_(storm)
			, is_initialized(false)
		{ }

	protected:
		void initialize(const char Filename[])
		{
			if (!is_initialized)
			{
				is_initialized = true;

				InterfaceStorm::error_code ec = 0;
				buffer buf(storm_.load(Filename, ec));
				if (ec == 0)
				{
					Reader::Read(std::move(buf), table_);
				}
			}
		}

		Table  table_;
		InterfaceStorm& storm_;
		bool   is_initialized;
	};

	class SLKLIB_API WtsConverter : public BaseConverter<WtsTable, WtsReader>
	{
		typedef BaseConverter<WtsTable, WtsReader> _Mybase;
	public:
		WtsConverter(InterfaceStorm& storm)
			: _Mybase(storm)
		{ }

		const std::string& Convert(const std::string& str)
		{
			_Mybase::initialize("war3map.wts");

			if (0 == str.compare(0, _countof("TRIGSTR_")-1, "TRIGSTR_"))
			{
				uint32_t index = Str2UInt(str.substr(_countof("TRIGSTR_")-1));
				auto It = _Mybase::table_.find(index);
				if (It != _Mybase::table_.end())
				{
					return  It->second;
				}
			}

			return str;
		}
	};

	class SLKLIB_API WesConverter : public BaseConverter<WesTable, WesReader>
	{
		typedef BaseConverter<WesTable, WesReader> _Mybase;
	public:
		WesConverter(InterfaceStorm& storm)
			: _Mybase(storm)
		{ }

		const std::string& Convert(const std::string& str)
		{
			_Mybase::initialize("UI\\WorldEditStrings.txt");
			
			if (0 == str.compare(0, _countof("WESTRING_")-1, "WESTRING_"))
			{
				auto It = _Mybase::table_.find(str);
				if (It != _Mybase::table_.end())
				{
					return It->second;
				}
			}

			return str;
		}
	};

	class SLKLIB_API DefaultValueConverter 
	{
	public:
		const std::string& Convert(const std::string& str) const
		{
			std::string tmp = trim_copy<std::string>(str.cbegin(), str.cend(), ctype::is_space());
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
	};

	struct Converter
	{
		WtsConverter          wts;
		WesConverter          wes;
		DefaultValueConverter def_val;

		Converter(InterfaceStorm& storm)
			: wts(storm)
			, wes(storm)
		{ }
	};
}
