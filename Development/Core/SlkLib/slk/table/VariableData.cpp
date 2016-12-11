#include <slk/table/VariableData.hpp>
#include <string>
#include <base/util/buffer.h>
#include <slk/utility/convert.h>
#include <base/exception/exception.h>

namespace slk
{
	VariableData::VariableData()
		:  _type(OBJTYPE_NONE)
	{ }

	VariableData::VariableData(VariableData const& that)
		: _type(that._type)
		, _i(that._i)
		, _f(that._f)
		, _s(that._s)
	{ }

	VariableData::Type VariableData::GetType() const
	{
		return _type;
	}

	void VariableData::SetType(const VariableData::Type& type)
	{
		this->_type = type;
	}

	void VariableData::LoadData(base::buffer_reader& reader)
	{
		// Get data
		switch (_type)
		{
		case OBJTYPE_INTEGER:
			_i = reader.read<uint32_t>();
			break;
		case OBJTYPE_REAL:
		case OBJTYPE_UNREAL:
			_f = reader.read<float>();
			break;
		case OBJTYPE_STRING:
			{
				std::error_code ec;
				_s = reader.read<std::string>(ec);
				if (!ec) _s.pop_back();
			}
			break;
		default:
			throw base::exception("Unexpected data type: %d.", _type);
			break;
		}
	}

	std::string VariableData::ToString() const
	{
		switch (_type)
		{
		case VariableData::OBJTYPE_INTEGER:
			return std::move(std::to_string((long long)_i));
		case VariableData::OBJTYPE_REAL:
		case VariableData::OBJTYPE_UNREAL:
			return std::move(std::to_string((long double)_f));
			break;
		case VariableData::OBJTYPE_STRING:
			return std::move(std::string(_s));
		default:
			return std::move(std::string("#VALUE!"));
		}
	}

	void VariableData::FromString(std::string const& str, Type type)
	{
		_type = type;
		switch (_type)
		{
		case VariableData::OBJTYPE_INTEGER:
			_i = Str2Int(str);
			break;
		case VariableData::OBJTYPE_REAL:
		case VariableData::OBJTYPE_UNREAL:
			_f = Str2Float(str);
			break;
		case VariableData::OBJTYPE_STRING:
			_s = str;
			break;
		default:
			break;
		}
	}
}
