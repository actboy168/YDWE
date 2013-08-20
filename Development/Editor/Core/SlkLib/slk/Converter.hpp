#pragma once

#include <slk/reader/WtsReader.hpp>
#include <slk/reader/WesReader.hpp>
#include <slk/InterfaceStorm.hpp>
#include <slk/utility/sequence.h>
#include <slk/utility/convert.h>

namespace slk
{
	template <class Table, class Reader>
	class BaseConverter
	{
	public:
		BaseConverter(InterfaceStorm& storm)
			: storm_(storm)
			, initialized_(false)
		{ }

		BaseConverter<Table, Reader>& operator =(BaseConverter<Table, Reader>& that)
		{
			storm_       = that.storm_;
			initialized_ = that.initialized_;			
			return *this;
		}

	protected:
		void initialize(const char Filename[])
		{
			if (!initialized_)
			{
				initialized_ = true;

				InterfaceStorm::error_code ec = 0;
				buffer buf(storm_.load(Filename, ec));
				if (ec == 0)
				{
					buffer_reader reader(buf);
					Reader::Read(reader, table_);
				}
			}
		}

		Table  table_;
		InterfaceStorm& storm_;
		bool   initialized_;
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
			std::string tmp = trim_copy<std::string>(str.cbegin(), str.cend());
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
