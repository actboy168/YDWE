#pragma once

#include <slk/port/config.h>
#include <string>
#include <base/util/buffer.h>

namespace slk
{
	class SLKLIB_API VariableData
	{
	public:
		enum Type
		{
			// no type
			OBJTYPE_NONE = -1,
			// bint
			OBJTYPE_INTEGER = 0,
			// 32 bit float
			OBJTYPE_REAL = 1,
			// Same as above, but 0 <= value <= 1
			OBJTYPE_UNREAL = 2,
			// Null terminated string
			OBJTYPE_STRING = 3,
			// ...
		};

	public:
		VariableData();
 		VariableData(VariableData const& that);
		Type GetType() const;
		void SetType(const Type& type);
		void LoadData(base::buffer_reader& reader);
		std::string ToString() const;
		void FromString(std::string const& str, Type type);

		template <typename SequenceT>
		void SaveData(SequenceT& buf) const
		{
			switch (_type)
			{
			case OBJTYPE_INTEGER:
				buf.append((typename SequenceT::value_type const*)&_i, sizeof(uint32_t));
				break;
			case OBJTYPE_REAL:
			case OBJTYPE_UNREAL:
				buf.append((typename SequenceT::value_type const*)&_f, sizeof(float));
				break;
			case OBJTYPE_STRING:
				buf.append(_s);
				buf.push_back('\0');
				break;
			default:
				throw base::exception("Unexpected data type: %d.", _type);
				break;
			}
		}

	private:
		Type                _type;
		uint32_t            _i;
		float               _f;
		std::string         _s;
	};
}
