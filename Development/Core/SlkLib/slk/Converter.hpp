#pragma once

#include <slk/reader/WtsReader.hpp>
#include <slk/reader/WesReader.hpp>
#include <slk/InterfaceStorm.hpp>
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
				base::buffer buf(storm_.load(Filename, ec));
				if (ec == 0 && buf.begin() != buf.end())
				{
					base::buffer_reader reader(buf);
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
		WtsConverter(InterfaceStorm& storm);
		const std::string& Convert(const std::string& str);
	};

	class SLKLIB_API WesConverter : public BaseConverter<WesTable, WesReader>
	{
		typedef BaseConverter<WesTable, WesReader> _Mybase;
	public:
		WesConverter(InterfaceStorm& storm);
		std::string_view Convert(const std::string_view& str);
	};

	class SLKLIB_API DefaultValueConverter 
	{
	public:
		std::string_view Convert(const std::string_view& str) const;
	};

	struct Converter
	{
		WtsConverter          wts;
		WesConverter          wes;
		DefaultValueConverter def_val;

		Converter(InterfaceStorm& storm);
	};
}
