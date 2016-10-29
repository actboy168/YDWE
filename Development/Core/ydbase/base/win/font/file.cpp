#include <base/win/font/file.h>
#include <base/file/memory_mapped_file.h>
#include <windows.h>

namespace base { namespace font {

	typedef struct _tagTT_OFFSET_TABLE
	{
		USHORT	uMajorVersion;
		USHORT	uMinorVersion;
		USHORT	uNumOfTables;
		USHORT	uSearchRange;
		USHORT	uEntrySelector;
		USHORT	uRangeShift;
	} TT_OFFSET_TABLE;

	typedef struct _tagTT_TABLE_DIRECTORY
	{
		UINT32	nTag;			    //table name
		ULONG	uCheckSum;			//Check sum
		ULONG	uOffset;			//Offset from beginning of file
		ULONG	uLength;			//length of the table in bytes
	} TT_TABLE_DIRECTORY;

	typedef struct _tagTT_NAME_TABLE_HEADER
	{
		USHORT	uFSelector;			//format selector. Always 0
		USHORT	uNRCount;			//Name Records count
		USHORT	uStorageOffset;		//Offset for strings storage, from start of the table
	} TT_NAME_TABLE_HEADER;

	typedef struct _tagTT_NAME_RECORD
	{
		USHORT	uPlatformID;
		USHORT	uEncodingID;
		USHORT	uLanguageID;
		USHORT	uNameID;
		USHORT	uStringLength;
		USHORT	uStringOffset;	//from start of storage area
	} TT_NAME_RECORD;

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

	std::string get_name_by_offset(const char* memory_ptr, size_t uOffset)
	{
		TT_NAME_TABLE_HEADER ttNTHeader;
		TT_NAME_RECORD       ttRecord;

		size_t index = uOffset;

		memcpy(&ttNTHeader, &memory_ptr[index], sizeof(TT_NAME_TABLE_HEADER));
		index += sizeof(TT_NAME_TABLE_HEADER);

		ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
		ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);

		for (size_t i = 0; i < ttNTHeader.uNRCount; ++i)
		{
			memcpy(&ttRecord, &memory_ptr[index], sizeof(TT_NAME_RECORD));
			index += sizeof(TT_NAME_RECORD);

			ttRecord.uPlatformID = SWAPWORD(ttRecord.uPlatformID);
			ttRecord.uEncodingID = SWAPWORD(ttRecord.uEncodingID);
			ttRecord.uLanguageID = SWAPWORD(ttRecord.uLanguageID);
			ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);
			ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
			ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);

			switch (ttRecord.uNameID)
			{
			case 1:
			{
				if (ttRecord.uEncodingID == 0)
				{
					const char* szString = &memory_ptr[uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset];
					return std::move(std::string(szString, ttRecord.uStringLength));
				}
				break;
			}
			default:
				break;
			}
		}

		return std::move(std::string());
	}

	std::string get_name(const char* memory_ptr)
	{
		TT_OFFSET_TABLE      ttOffsetTable;
		TT_TABLE_DIRECTORY   tblDir;
		size_t index = 0;

		memcpy(&ttOffsetTable, &memory_ptr[index], sizeof(TT_OFFSET_TABLE));
		index += sizeof(TT_OFFSET_TABLE);

		ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
		ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
		ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

		//check is this is a true type font and the version is 1.0
		if (ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
		{
			return std::move(std::string());
		}

		memset(&tblDir, 0, sizeof(TT_TABLE_DIRECTORY));

		for (size_t i = 0; i< ttOffsetTable.uNumOfTables; ++i)
		{
			memcpy(&tblDir, &memory_ptr[index], sizeof(TT_TABLE_DIRECTORY));
			index += sizeof(TT_TABLE_DIRECTORY);

			if (tblDir.nTag == 'eman')
			{
				tblDir.uLength = SWAPLONG(tblDir.uLength);
				tblDir.uOffset = SWAPLONG(tblDir.uOffset);
				return get_name_by_offset(memory_ptr, tblDir.uOffset);
			}
			else if (tblDir.nTag == 0)
			{
				break;
			}
		}

		return std::move(std::string());
	}

	std::string get_name(const fs::path& fil_path)
	{
		try
		{
			base::file::memory_mapped_file mmap(fil_path.c_str());
			const char* memory_ptr = static_cast<const char*>(mmap.memory());
			if (memory_ptr)
			{
				return get_name(memory_ptr);
			}
		}
		catch (std::exception const&) {}

		return std::move(std::string());
	}
}}
