#pragma once

#include <windows.h>

#include <string>

#include <unzip.h>
#include <zip.h>

namespace zip 
{
	namespace internal 
	{
		unzFile OpenForUnzipping(const std::string& file_name_utf8);
		unzFile OpenHandleForUnzipping(HANDLE zip_handle);
		unzFile PreprareMemoryForUnzipping(const std::string& data);
		zipFile OpenForZipping(const std::string& file_name_utf8, int append_flag);

		const int kZipMaxPath = 256;
		const int kZipBufSize = 8192;
	} 
}
