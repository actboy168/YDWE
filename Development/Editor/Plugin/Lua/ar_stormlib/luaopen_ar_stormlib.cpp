#define __STORMLIB_SELF__
#include <StormLib.h>
#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)
#include <boost/filesystem.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scope_exit.hpp>

namespace fs = boost::filesystem;

namespace NLuaAPI { namespace NMPQ {
	static void *LuaMpqStormLibCreateArchive(const fs::path &mpqPath, boost::uint32_t /*dwFlags*/, boost::uint32_t dwMaxFileCount)
	{
		SFILE_CREATE_MPQ CreateInfo = {0};
		CreateInfo.cbSize         = sizeof(SFILE_CREATE_MPQ);
		CreateInfo.dwMpqVersion   = MPQ_FORMAT_VERSION_1;
		CreateInfo.dwStreamFlags  = STREAM_PROVIDER_LINEAR | BASE_PROVIDER_FILE;
		CreateInfo.dwAttrFlags    = 0;
		CreateInfo.dwSectorSize   = 0x10000;
		CreateInfo.dwRawChunkSize = 0;
		CreateInfo.dwMaxFileCount = dwMaxFileCount;

		HANDLE mpqHandle;
		if (!SFileCreateArchive2(mpqPath.string().c_str(), &CreateInfo, &mpqHandle))
		{
			mpqHandle = NULL;
		}

		return mpqHandle;
	}

	static void *LuaMpqStormLibOpenArchive(const fs::path &mpqPath, boost::uint32_t priority, boost::uint32_t flags)
	{
		HANDLE mpqHandle;

		bool ret = SFileOpenArchive(
			mpqPath.string().c_str(),
			priority,
			flags,
			&mpqHandle
			);

		if (!ret)
		{
			mpqHandle = NULL;
		}

		return mpqHandle;
	}

	static LCID LuaMpqStormLibSetLocale(LCID newLocale)
	{
		return SFileSetLocale(newLocale);
	}

	static LCID LuaMpqStormLibGetLocale()
	{
		return SFileGetLocale();
	}

	static boost::int32_t LuaMpqStormLibAddListFile(void *mpqHandle, const char *listFile)
	{
		return SFileAddListFile(
			reinterpret_cast<HANDLE>(mpqHandle),
			listFile
			);
	}

	static boost::int32_t LuaMpqStormLibGetAttributes(void *mpqHandle)
	{
		return SFileGetAttributes(reinterpret_cast<HANDLE>(mpqHandle));
	}

	static bool LuaMpqStormLibSetAttributes(void *mpqHandle, boost::uint32_t flags)
	{
		return SFileSetAttributes(reinterpret_cast<HANDLE>(mpqHandle), flags);
	}

	static bool LuaMpqStormLibUpdateFileAttributes(void *mpqHandle, const char *file)
	{
		return SFileUpdateFileAttributes(
			reinterpret_cast<HANDLE>(mpqHandle),
			file
			);
	}

	static bool LuaMpqStormLibOpenPatchArchive(void *mpqHandle, const char *patchMpqName, const char *patchPathPrefix, boost::uint32_t flags)
	{
		return SFileOpenPatchArchive(
			reinterpret_cast<HANDLE>(mpqHandle),
			patchMpqName,
			patchPathPrefix,
			flags
			);
	}

	static bool LuaMpqStormLibIsPatchedArchive(void *mpqHandle)
	{
		return SFileIsPatchedArchive(reinterpret_cast<HANDLE>(mpqHandle));
	}

	static void *LuaMpqStormLibOpenFileEx(void *mpqHandle, const char *fileName, boost::uint32_t searchScope)
	{
		HANDLE fileHandle;

		bool ret = SFileOpenFileEx(
			reinterpret_cast<HANDLE>(mpqHandle),
			fileName,
			searchScope,
			&fileHandle
			);

		if (!ret)
		{
			fileHandle = 0;
		}

		return fileHandle;
	}

	static void LuaMpqStormLibGetFileSize(lua_State *pState, void *fileHandle)
	{
		DWORD fileSizeHigh;
		boost::uint32_t fileSizeLow = SFileGetFileSize(
			reinterpret_cast<HANDLE>(fileHandle),
			&fileSizeHigh
			);

		lua_pushnumber(pState, fileSizeLow);
		lua_pushnumber(pState, fileSizeHigh);
	}

	static void LuaMpqStormLibSetFilePointer(lua_State *pState, void *fileHandle, boost::uint32_t positionLow, boost::uint32_t positionHigh, boost::uint32_t moveMethod)
	{
		LONG fileSizeHigh = positionHigh;
		boost::uint32_t fileSizeLow = SFileSetFilePointer(
			reinterpret_cast<HANDLE>(fileHandle),
			positionLow,
			&fileSizeHigh,
			moveMethod
			);

		lua_pushnumber(pState, fileSizeLow);
		lua_pushnumber(pState, fileSizeHigh);
	}

	static void LuaMpqStormLibReadFile(lua_State *pState, void *fileHandle, size_t readSize)
	{
		boost::scoped_array<char> buffer(new char[readSize]);
		DWORD actualReadSize;

		bool result = SFileReadFile(
			reinterpret_cast<HANDLE>(fileHandle),
			buffer.get(),
			readSize,
			&actualReadSize,
			0
			);

		lua_pushboolean(pState, result);
		if (result && actualReadSize)
			lua_pushlstring(pState, buffer.get(), actualReadSize);
		else
			lua_pushnil(pState);
	}

	static bool LuaMpqStormLibCloseFile(void *fileHandle)
	{
		return SFileCloseFile(reinterpret_cast<HANDLE>(fileHandle));
	}

	static void LuaMpqStormLibGetFileName(lua_State *pState, void *fileHandle)
	{
		char buffer[MAX_PATH];

		bool result = SFileGetFileName(reinterpret_cast<HANDLE>(fileHandle), buffer);
		if (result)
			lua_pushfstring(pState, "%s", buffer);
		else
			lua_pushnil(pState);
	}

	static bool LuaMpqStormLibAddFile(void *mpqHandle, const fs::path &srcPath, const std::string &pathInMpq, boost::uint32_t flags)
	{
		return SFileAddFile(
			reinterpret_cast<HANDLE>(mpqHandle),
			srcPath.string().c_str(),
			pathInMpq.c_str(),
			flags
			);
	}

	static bool LuaMpqStormLibAddWave(void *mpqHandle, const fs::path &srcPath, const std::string &pathInMpq, boost::uint32_t flags, boost::uint32_t quality)
	{
		return SFileAddWave(
			reinterpret_cast<HANDLE>(mpqHandle),
			srcPath.string().c_str(),
			pathInMpq.c_str(),
			flags,
			quality
			);
	}

	static bool LuaMpqStormLibAddFileEx(void *mpqHandle, const fs::path &srcPath, const std::string &pathInMpq, boost::uint32_t flags, boost::uint32_t compression, boost::uint32_t compressionNext)
	{
		return SFileAddFileEx(
			reinterpret_cast<HANDLE>(mpqHandle),
			srcPath.string().c_str(),
			pathInMpq.c_str(),
			flags,
			compression,
			compressionNext
			);
	}

// 	static void *LuaMpqStormLibCreateFile(void *mpqHandle, const char *fileName, const NMPIR::mpir_integer &fileTimeMpir, boost::uint32_t fileSize, boost::uint32_t locale, boost::uint32_t flag)
// 	{
// 		HANDLE fileHandle;
// 		mpz_class fileTimeBigInt(fileTimeMpir.get_mpz_class());
// 		LARGE_INTEGER fileTime;
// 
// 		fileTime.LowPart = mpz_class(fileTimeBigInt & 0xFFFFFFFFU).get_ui();
// 		fileTime.HighPart = mpz_class(fileTimeBigInt >> 32).get_ui();
// 
// 		BOOST_STATIC_ASSERT(sizeof(fileTime) == sizeof(FILETIME));
// 
// 		bool ret = SFileCreateFile(
// 			reinterpret_cast<HANDLE>(mpqHandle),
// 			fileName,
// 			fileTime.QuadPart,
// 			fileSize,
// 			locale,
// 			flag,
// 			&fileHandle
// 			);
// 
// 		if (!ret)
// 		{
// 			fileHandle = 0;
// 		}
// 
// 		return fileHandle;
// 	}

	static bool LuaMpqStormLibWriteFile(void *fileHandle, const std::string &data, boost::uint32_t compression)
	{
		return SFileWriteFile(
			reinterpret_cast<HANDLE>(fileHandle),
			data.data(),
			data.size(),
			compression
			);
	}

	static bool LuaMpqStormLibFinishFile(void *fileHandle)
	{
		return SFileFinishFile(reinterpret_cast<HANDLE>(fileHandle));
	}

	static bool LuaMpqStormLibSetFileLocale(void *fileHandle, boost::uint32_t locale)
	{
		return SFileSetFileLocale(reinterpret_cast<HANDLE>(fileHandle), locale);
	}

	static bool LuaMpqStormLibSetDataCompression(boost::uint32_t compression)
	{
		return SFileSetDataCompression(compression);
	}

	static bool LuaMpqStormLibRemoveFile(void *mpqHandle, const std::string &pathInMpq, boost::uint32_t searchScope)
	{
		return SFileRemoveFile(
			reinterpret_cast<HANDLE>(mpqHandle),
			pathInMpq.c_str(),
			searchScope
			);
	}

	static bool LuaMpqStormLibRenameFile(void *mpqHandle, const std::string &oldPathInMpq, const std::string &newPathInMpq)
	{
		return SFileRenameFile(
			reinterpret_cast<HANDLE>(mpqHandle),
			oldPathInMpq.c_str(),
			newPathInMpq.c_str()
			);
	}

	static bool LuaMpqStormLibExtractFile(void *mpqHandle, const fs::path &filePath, const std::string &pathInMpq)
	{
		return SFileExtractFile(
			reinterpret_cast<HANDLE>(mpqHandle),
			pathInMpq.c_str(),
			filePath.string().c_str(),
			SFILE_OPEN_FROM_MPQ 
			);
	}

	static bool LuaMpqStormLibHasFile(void *mpqHandle, const std::string &pathInMpq)
	{
		return SFileHasFile(
			reinterpret_cast<HANDLE>(mpqHandle),
			const_cast<char *>(pathInMpq.c_str())
			);
	}

	static bool LuaMpqStormLibCompactArchive(void *mpqHandle, const luabind::object &listFile)
	{
		boost::optional<std::string> optListFile;

		return SFileCompactArchive(
			reinterpret_cast<HANDLE>(mpqHandle),
			(optListFile = luabind::object_cast_nothrow<std::string>(listFile))
			? optListFile->c_str()
			: NULL,
			0
			);
	}

	static bool LuaMpqStormLibCloseArchive(void *mpqHandle)
	{
		return SFileCloseArchive(
			reinterpret_cast<HANDLE>(mpqHandle)
			);
	}

	static bool LuaMpqStormLibFlushArchive(void *mpqHandle)
	{
		return SFileFlushArchive(
			reinterpret_cast<HANDLE>(mpqHandle)
			);
	}

	static boost::uint32_t LuaMpqStormLibVerifyFile(void *mpqHandle, const char *fileName, boost::uint32_t flags)
	{
		return SFileVerifyFile(reinterpret_cast<HANDLE>(mpqHandle), fileName, flags);
	}

	static boost::uint32_t LuaMpqStormLibVerifyArchive(void *mpqHandle)
	{
		return SFileVerifyArchive(reinterpret_cast<HANDLE>(mpqHandle));
	}


	static void LuaMpqStormLibLoadFile(lua_State *pState, void* mpqHandle, const std::string &pathInMpq)
	{
		try
		{
			HANDLE fileHandle = NULL;
			BOOL ret = SFileOpenFileEx(reinterpret_cast<HANDLE>(mpqHandle), pathInMpq.c_str(), 0, &fileHandle);

			BOOST_SCOPE_EXIT( (ret) (&fileHandle) )
			{
				if (ret)
				{
					SFileCloseFile(fileHandle);
				}
			} BOOST_SCOPE_EXIT_END

			if (ret)
			{
				size_t fileSize = SFileGetFileSize(fileHandle, NULL);
				boost::scoped_array<char> buffer(new char[fileSize]);

				ret = SFileReadFile(fileHandle, buffer.get(), fileSize, (LPDWORD)&fileSize, NULL);
				if (ret)
				{
					luabind::object(pState, true).push(pState);
					luabind::object(pState, std::string(buffer.get(), fileSize)).push(pState);
					return ;
				}
			}
		}
		catch (...)
		{
		}

		luabind::object(pState, false).push(pState);
	}
}}

int luaopen_ar_stormlib(lua_State *pState)
{
	using namespace luabind;

	// Bind MPQ
	module(pState, "ar")
	[
		// Stormlib
		namespace_("stormlib")
		[
			def("create_archive",         &NLuaAPI::NMPQ::LuaMpqStormLibCreateArchive),
			def("open_archive",           &NLuaAPI::NMPQ::LuaMpqStormLibOpenArchive),
			def("set_locale",             &NLuaAPI::NMPQ::LuaMpqStormLibSetLocale),
			def("get_locale",             &NLuaAPI::NMPQ::LuaMpqStormLibGetLocale),
			def("add_list_file",          &NLuaAPI::NMPQ::LuaMpqStormLibAddListFile),
			def("get_attributes",         &NLuaAPI::NMPQ::LuaMpqStormLibGetAttributes),
			def("set_attributes",         &NLuaAPI::NMPQ::LuaMpqStormLibSetAttributes),
			def("update_file_attributes", &NLuaAPI::NMPQ::LuaMpqStormLibUpdateFileAttributes),
			def("open_patch_archive",     &NLuaAPI::NMPQ::LuaMpqStormLibOpenPatchArchive),
			def("is_patched_archive",     &NLuaAPI::NMPQ::LuaMpqStormLibIsPatchedArchive),
			def("open_file_ex",           &NLuaAPI::NMPQ::LuaMpqStormLibOpenFileEx),
			def("get_file_size",          &NLuaAPI::NMPQ::LuaMpqStormLibGetFileSize),
			def("set_file_pointer",       &NLuaAPI::NMPQ::LuaMpqStormLibSetFilePointer),
			def("read_file",              &NLuaAPI::NMPQ::LuaMpqStormLibReadFile),
			def("close_file",             &NLuaAPI::NMPQ::LuaMpqStormLibCloseFile),
			def("get_file_name",          &NLuaAPI::NMPQ::LuaMpqStormLibGetFileName),
			def("remove_file",            &NLuaAPI::NMPQ::LuaMpqStormLibRemoveFile),
			def("add_file",               &NLuaAPI::NMPQ::LuaMpqStormLibAddFile),
			def("add_wave",               &NLuaAPI::NMPQ::LuaMpqStormLibAddWave),
			def("add_file_ex",            &NLuaAPI::NMPQ::LuaMpqStormLibAddFileEx),
			//def("create_file",            &NLuaAPI::NMPQ::LuaMpqStormLibCreateFile),
			def("write_file",             &NLuaAPI::NMPQ::LuaMpqStormLibWriteFile),
			def("finish_file",            &NLuaAPI::NMPQ::LuaMpqStormLibFinishFile),
			def("set_file_locale",        &NLuaAPI::NMPQ::LuaMpqStormLibSetFileLocale),
			def("set_data_compression",   &NLuaAPI::NMPQ::LuaMpqStormLibSetDataCompression),
			def("rename_file",            &NLuaAPI::NMPQ::LuaMpqStormLibRenameFile),
			def("extract_file",           &NLuaAPI::NMPQ::LuaMpqStormLibExtractFile),
			def("has_file",               &NLuaAPI::NMPQ::LuaMpqStormLibHasFile),
			def("compact_archive",        &NLuaAPI::NMPQ::LuaMpqStormLibCompactArchive),
			def("close_archive",          &NLuaAPI::NMPQ::LuaMpqStormLibCloseArchive),
			def("flush_archive",          &NLuaAPI::NMPQ::LuaMpqStormLibFlushArchive),
			def("verify_file",            &NLuaAPI::NMPQ::LuaMpqStormLibVerifyFile),
			def("verify_archive",         &NLuaAPI::NMPQ::LuaMpqStormLibVerifyArchive),
			def("load_file",              &NLuaAPI::NMPQ::LuaMpqStormLibLoadFile)
		]
	];

	lua_getglobal(pState, "ar");
	lua_getfield(pState, -1, "stormlib");
	object constantTable(from_stack(pState, -1));
	lua_pop(pState, 1);

#define LUA_AREA_CONSTANT(name) constantTable[#name] = name

	LUA_AREA_CONSTANT(ID_MPQ);
	LUA_AREA_CONSTANT(ID_MPQ_USERDATA);
	LUA_AREA_CONSTANT(ERROR_AVI_FILE);
	LUA_AREA_CONSTANT(ERROR_UNKNOWN_FILE_KEY);
	LUA_AREA_CONSTANT(ERROR_CHECKSUM_ERROR);
	LUA_AREA_CONSTANT(ERROR_INTERNAL_FILE);
	LUA_AREA_CONSTANT(HASH_TABLE_SIZE_MIN);
	LUA_AREA_CONSTANT(HASH_TABLE_SIZE_MAX);
	LUA_AREA_CONSTANT(HASH_ENTRY_DELETED);
	LUA_AREA_CONSTANT(HASH_ENTRY_FREE);
	LUA_AREA_CONSTANT(HASH_STATE_SIZE);
	LUA_AREA_CONSTANT(MPQ_PATCH_PREFIX_LEN);
	LUA_AREA_CONSTANT(STREAM_FLAG_READ_ONLY);
	LUA_AREA_CONSTANT(STREAM_FLAG_WRITE_SHARE);
	LUA_AREA_CONSTANT(STREAM_FLAG_MASK);
	LUA_AREA_CONSTANT(STREAM_OPTIONS_MASK);
	LUA_AREA_CONSTANT(SFILE_OPEN_HARD_DISK_FILE);
	LUA_AREA_CONSTANT(SFILE_OPEN_CDROM_FILE);
	LUA_AREA_CONSTANT(SFILE_OPEN_FROM_MPQ);
	LUA_AREA_CONSTANT(SFILE_OPEN_BASE_FILE);
	LUA_AREA_CONSTANT(SFILE_OPEN_ANY_LOCALE);
	LUA_AREA_CONSTANT(SFILE_OPEN_LOCAL_FILE);
	LUA_AREA_CONSTANT(MPQ_FLAG_READ_ONLY);
	LUA_AREA_CONSTANT(MPQ_FLAG_CHANGED);
	LUA_AREA_CONSTANT(MPQ_FLAG_PROTECTED);
	LUA_AREA_CONSTANT(MPQ_FLAG_CHECK_SECTOR_CRC);
	LUA_AREA_CONSTANT(MPQ_FLAG_NEED_FIX_SIZE);
	LUA_AREA_CONSTANT(SFILE_INVALID_SIZE);
	LUA_AREA_CONSTANT(SFILE_INVALID_POS);
	LUA_AREA_CONSTANT(SFILE_INVALID_ATTRIBUTES);
	LUA_AREA_CONSTANT(MPQ_FILE_IMPLODE);
	LUA_AREA_CONSTANT(MPQ_FILE_COMPRESS);
	LUA_AREA_CONSTANT(MPQ_FILE_COMPRESSED);
	LUA_AREA_CONSTANT(MPQ_FILE_ENCRYPTED);
	LUA_AREA_CONSTANT(MPQ_FILE_FIX_KEY);
	LUA_AREA_CONSTANT(MPQ_FILE_PATCH_FILE);
	LUA_AREA_CONSTANT(MPQ_FILE_SINGLE_UNIT);
	LUA_AREA_CONSTANT(MPQ_FILE_DELETE_MARKER);
	LUA_AREA_CONSTANT(MPQ_FILE_SECTOR_CRC);
	LUA_AREA_CONSTANT(MPQ_FILE_EXISTS);
	LUA_AREA_CONSTANT(MPQ_FILE_REPLACEEXISTING);
	LUA_AREA_CONSTANT(MPQ_FILE_VALID_FLAGS);
	LUA_AREA_CONSTANT(MPQ_COMPRESSION_HUFFMANN);
	LUA_AREA_CONSTANT(MPQ_COMPRESSION_ZLIB);
	LUA_AREA_CONSTANT(MPQ_COMPRESSION_PKWARE);
	LUA_AREA_CONSTANT(MPQ_COMPRESSION_BZIP2);
	LUA_AREA_CONSTANT(MPQ_COMPRESSION_SPARSE);
	LUA_AREA_CONSTANT(MPQ_COMPRESSION_ADPCM_MONO);
	LUA_AREA_CONSTANT(MPQ_COMPRESSION_ADPCM_STEREO);
	LUA_AREA_CONSTANT(MPQ_COMPRESSION_LZMA);
	LUA_AREA_CONSTANT(MPQ_WAVE_QUALITY_HIGH);
	LUA_AREA_CONSTANT(MPQ_WAVE_QUALITY_MEDIUM);
	LUA_AREA_CONSTANT(MPQ_WAVE_QUALITY_LOW);
	LUA_AREA_CONSTANT(SFILE_INFO_ARCHIVE_NAME);
	LUA_AREA_CONSTANT(SFILE_INFO_ARCHIVE_SIZE);
	LUA_AREA_CONSTANT(SFILE_INFO_HASH_TABLE_SIZE);
	LUA_AREA_CONSTANT(SFILE_INFO_BLOCK_TABLE_SIZE);
	LUA_AREA_CONSTANT(SFILE_INFO_SECTOR_SIZE);
	LUA_AREA_CONSTANT(SFILE_INFO_HASH_TABLE);
	LUA_AREA_CONSTANT(SFILE_INFO_BLOCK_TABLE);
	LUA_AREA_CONSTANT(SFILE_INFO_NUM_FILES);
	LUA_AREA_CONSTANT(SFILE_INFO_STREAM_FLAGS);
	LUA_AREA_CONSTANT(SFILE_INFO_IS_READ_ONLY);
	LUA_AREA_CONSTANT(SFILE_INFO_HASH_INDEX);
	LUA_AREA_CONSTANT(SFILE_INFO_CODENAME1);
	LUA_AREA_CONSTANT(SFILE_INFO_CODENAME2);
	LUA_AREA_CONSTANT(SFILE_INFO_LOCALEID);
	LUA_AREA_CONSTANT(SFILE_INFO_BLOCKINDEX);
	LUA_AREA_CONSTANT(SFILE_INFO_FILE_SIZE);
	LUA_AREA_CONSTANT(SFILE_INFO_COMPRESSED_SIZE);
	LUA_AREA_CONSTANT(SFILE_INFO_FLAGS);
	LUA_AREA_CONSTANT(SFILE_INFO_POSITION);
	LUA_AREA_CONSTANT(SFILE_INFO_KEY);
	LUA_AREA_CONSTANT(SFILE_INFO_KEY_UNFIXED);
	LUA_AREA_CONSTANT(SFILE_INFO_FILETIME);
	LUA_AREA_CONSTANT(MPQ_ATTRIBUTE_CRC32);
	LUA_AREA_CONSTANT(MPQ_ATTRIBUTE_FILETIME);
	LUA_AREA_CONSTANT(MPQ_ATTRIBUTE_MD5);
	LUA_AREA_CONSTANT(MPQ_ATTRIBUTE_ALL);
	LUA_AREA_CONSTANT(MPQ_ATTRIBUTES_V1);
	LUA_AREA_CONSTANT(MPQ_OPEN_NO_LISTFILE);
	LUA_AREA_CONSTANT(MPQ_OPEN_NO_ATTRIBUTES);
	LUA_AREA_CONSTANT(MPQ_OPEN_FORCE_MPQ_V1);
	LUA_AREA_CONSTANT(MPQ_OPEN_CHECK_SECTOR_CRC);
	LUA_AREA_CONSTANT(MPQ_OPEN_READ_ONLY);
	LUA_AREA_CONSTANT(MPQ_OPEN_ENCRYPTED);
	LUA_AREA_CONSTANT(MPQ_CREATE_ATTRIBUTES);
	LUA_AREA_CONSTANT(MPQ_CREATE_ARCHIVE_V1);
	LUA_AREA_CONSTANT(MPQ_CREATE_ARCHIVE_V2);
	LUA_AREA_CONSTANT(MPQ_CREATE_ARCHIVE_V3);
	LUA_AREA_CONSTANT(MPQ_CREATE_ARCHIVE_V4);
	LUA_AREA_CONSTANT(MPQ_CREATE_ARCHIVE_VMASK);
	LUA_AREA_CONSTANT(VERIFY_OPEN_ERROR);
	LUA_AREA_CONSTANT(VERIFY_READ_ERROR);
	LUA_AREA_CONSTANT(VERIFY_FILE_HAS_CHECKSUM);
	LUA_AREA_CONSTANT(VERIFY_FILE_CHECKSUM_ERROR);
	LUA_AREA_CONSTANT(VERIFY_FILE_HAS_MD5);
	LUA_AREA_CONSTANT(VERIFY_FILE_MD5_ERROR);
	LUA_AREA_CONSTANT(SFILE_VERIFY_MPQ_HEADER);
	LUA_AREA_CONSTANT(SFILE_VERIFY_HET_TABLE);
	LUA_AREA_CONSTANT(SFILE_VERIFY_BET_TABLE);
	LUA_AREA_CONSTANT(SFILE_VERIFY_HASH_TABLE);
	LUA_AREA_CONSTANT(SFILE_VERIFY_BLOCK_TABLE);
	LUA_AREA_CONSTANT(SFILE_VERIFY_HIBLOCK_TABLE);
	LUA_AREA_CONSTANT(SFILE_VERIFY_FILE);
	LUA_AREA_CONSTANT(ERROR_NO_SIGNATURE);
	LUA_AREA_CONSTANT(ERROR_VERIFY_FAILED);
	LUA_AREA_CONSTANT(ERROR_WEAK_SIGNATURE_OK);
	LUA_AREA_CONSTANT(ERROR_WEAK_SIGNATURE_ERROR);
	LUA_AREA_CONSTANT(ERROR_STRONG_SIGNATURE_OK);
	LUA_AREA_CONSTANT(ERROR_STRONG_SIGNATURE_ERROR);
	LUA_AREA_CONSTANT(CCB_CHECKING_FILES);
	LUA_AREA_CONSTANT(CCB_CHECKING_HASH_TABLE);
	LUA_AREA_CONSTANT(CCB_COPYING_NON_MPQ_DATA);
	LUA_AREA_CONSTANT(CCB_COMPACTING_FILES);
	LUA_AREA_CONSTANT(CCB_CLOSING_ARCHIVE);

	return 0;
}
