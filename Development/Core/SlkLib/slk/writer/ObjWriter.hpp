#pragma once

#include <base/util/buffer.h>
#include <slk/table/ObjTable.hpp>

namespace slk
{
	class ObjWriter
	{
	public:
		template <typename SequenceT, class _Ty>
		static void WriteValue(SequenceT& buf, _Ty const& val)
		{
			buf.append((typename SequenceT::value_type const*)&val, sizeof(_Ty));
		}

		template <typename SequenceT>
		static SequenceT WriteAttributeTable(object_id const& ObjId, AttributeTable<ObjectWithOptinal> const& attributes)
		{
			SequenceT output;
			uint32_t nCount = 0;

			WriteValue<SequenceT, uint32_t>(output, 0);
			foreach (auto const& AttributeCatalogIt, attributes)
			{
				foreach (auto const& AttributeIt, AttributeCatalogIt.second)
				{
					Attribute<ObjectWithOptinal> const& attr = AttributeIt.second;
					WriteValue<SequenceT, uint32_t>(output, attr.GetId());
					VariableData data = attr.GetData();
					WriteValue<SequenceT, VariableData::Type>(output, data.GetType());
					WriteValue<SequenceT, uint32_t>(output, attr.GetLevel());
					WriteValue<SequenceT, uint32_t>(output, attr.GetDataIndicator());
					data.SaveData<SequenceT>(output);
					WriteValue<SequenceT, uint32_t>(output, ObjId);

					nCount++;
				}
			}

			*(uint32_t*)&*output.begin() = nCount;

			return std::move(output);
		}

		template <typename SequenceT>
		static SequenceT WriteAttributeTable(object_id const& ObjId, AttributeTable<ObjectWithoutOptinal> const& attributes)
		{
			SequenceT output;
			uint32_t nCount = 0;

			WriteValue<SequenceT, uint32_t>(output, 0);
			foreach (auto const& AttributeIt, attributes)
			{
				Attribute<ObjectWithoutOptinal> const& attr = AttributeIt.second;
				WriteValue<SequenceT, uint32_t>(output, attr.GetId());
				VariableData data = attr.GetData();
				WriteValue<SequenceT, VariableData::Type>(output, data.GetType());

				data.SaveData<SequenceT>(output);
				WriteValue<SequenceT, uint32_t>(output, ObjId);

				nCount++;
			}

			*(uint32_t*)&*output.begin() = nCount;

			return std::move(output);
		}

		template <typename SequenceT, class ObjTableT>
		static SequenceT Write(ObjTableT const& table)
		{
			std::string OriginalBuffer;
			std::string CustomBuffer;
			size_t nOriginalCount = 0;
			size_t nCustomCount = 0;

			foreach (auto const& ObjectIt, table)
			{
				ObjSingle<ObjTableT::table_type> const& object = ObjectIt.second;

				if (object.GetId() == object.GetBaseId())
				{
					WriteValue<std::string, uint32_t>(OriginalBuffer, object.GetId());
					WriteValue<std::string, uint32_t>(OriginalBuffer, 0);
					OriginalBuffer += WriteAttributeTable<std::string>(object.GetId(), object.GetAttributes());
					nOriginalCount++;
				}
				else
				{
					WriteValue<std::string, uint32_t>(CustomBuffer, object.GetId());
					WriteValue<std::string, uint32_t>(CustomBuffer, object.GetBaseId());
					CustomBuffer += WriteAttributeTable<std::string>(object.GetId(), object.GetAttributes());
					nCustomCount++;
				}
			}

			std::string Buffer;
			WriteValue<std::string, uint32_t>(Buffer, 2); // W3OµÄ°æ±¾ºÅ
			WriteValue<std::string, uint32_t>(Buffer, nOriginalCount);
			Buffer += OriginalBuffer;
			WriteValue<std::string, uint32_t>(Buffer, nCustomCount);
			Buffer += CustomBuffer;

			return std::move(SequenceT(Buffer));
		}
	};
}
