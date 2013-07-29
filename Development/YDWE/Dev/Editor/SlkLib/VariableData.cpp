#include "VariableData.hpp"
#include <string>
#include "Util.hpp"
#include "Buffer.hpp"
#include <ydwe/exception/exception.h>

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

	void VariableData::LoadData(buffer& buf)
	{
		// Get data
		switch (_type)
		{
		case OBJTYPE_INTEGER:
			_i = buf.read<uint32_t>();
			break;
		case OBJTYPE_REAL:
		case OBJTYPE_UNREAL:
			_f = buf.read<float>();
			break;
		case OBJTYPE_STRING:
			{
				buffer::status ec = buffer::normal;
				_s = buf.read<std::string>(ec);
				if (ec ==  buffer::normal) _s.pop_back();
			}
			break;
		default:
			throw ydwe::exception(L"Unexpected data type: %d.", _type);
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
