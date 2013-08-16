#pragma once

#include <string>
#include <slk/table/BaseTable.hpp>

namespace slk
{
	class SLKLIB_API SlkValue
	{
	public:
		SlkValue()
			: val_()
			, is_str_(true)
		{ }

		SlkValue(const std::string& val)
		{
			size_t slen = val.size();
			if (slen >= 2 && val[0] == '"' && val[slen-1] == '"')
			{
				is_str_ = true;
				val_.assign(val, 1, slen-2);
			}
			else
			{
				is_str_ = false;
				val_.assign(val);
			}
		}

		SlkValue(const std::string& val, bool is_str)
			: val_(val)
			, is_str_(is_str)
		{ }

		std::string const& to_string() const
		{
			return val_;
		}

		void set(const std::string& val)
		{
			val_ = val;
		}

		bool is_str() const
		{
			return is_str_;
		}

	private:
		std::string val_;
		bool is_str_;
	};

	class SLKLIB_API SlkSingle : public HashTable<std::string, SlkValue>::IType
	{
	};

	class SLKLIB_API SlkTable : public HashTable<object_id, SlkSingle>::Type
	{
	};
}
