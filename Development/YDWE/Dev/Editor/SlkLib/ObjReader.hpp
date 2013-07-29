#pragma once

#include "Buffer.hpp"
#include "ObjectId.hpp"
#include "ObjTable.hpp"

namespace slk
{
	class ObjReader
	{
	public:
		template <class ObjTableT>
		static void Read(buffer&& buf, ObjTableT& table)
		{
			buf.read<W3OHeader>();
			ReadOriginalTable(buf, table);
			ReadCustomTable(buf, table);
		}

	private:
		struct W3OHeader
		{
			uint32_t nVersion;
		};

		template <class ObjTableT>
		static void ReadOriginalTable(buffer& buf, ObjTableT& table)
		{
			// Original objects table
			uint32_t nOriginalTableCount = buf.read<uint32_t>();
			for (uint32_t i = 0; i < nOriginalTableCount; ++i)
			{
				// Original Id
				ObjectId OriginalObjectId(buf.read<uint32_t>(), ObjectId::not_swap_t());
				// New id, always be 0
				ObjectId NewObjectId(buf.read<uint32_t>(), ObjectId::not_swap_t());

				assert (OriginalObjectId.vaild());

				if (NewObjectId != 0)
				{
					throw ydwe::exception(L"Object in original table has incorrect id(Not 0).");
				}

				ObjSingle<ObjTableT::table_type>& obj = table[OriginalObjectId];
				obj.SetId(OriginalObjectId);
				obj.SetBaseId(OriginalObjectId);

				ReadModData<ObjTableT::table_type>(buf, obj);
			}
		}

		template <class ObjTableT>
		static void ReadCustomTable(buffer& buf, ObjTableT& table)
		{
			// Custom objects table
			uint32_t nCustomTableCount = buf.read<uint32_t>();
			for (uint32_t i = 0; i < nCustomTableCount; ++i)
			{
				// Original Id
				ObjectId OriginalObjectId(buf.read<uint32_t>(), ObjectId::not_swap_t());
				// New id
				ObjectId NewObjectId(buf.read<uint32_t>(), ObjectId::not_swap_t());

				assert (OriginalObjectId.vaild());
				assert (NewObjectId.vaild());

				ObjSingle<ObjTableT::table_type>& obj = table[NewObjectId];
				obj.SetId(NewObjectId);
				obj.SetBaseId(OriginalObjectId);

				ReadModData<ObjTableT::table_type>(buf, obj);
			}
		}

		template <OBJECT_PARSER_OPTION Option>
		static void ReadModData(buffer& buf, ObjSingle<Option>& obj);
	};

}
