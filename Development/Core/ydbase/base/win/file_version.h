#pragma once

#include <base/config.h>
#include <Windows.h>
#include <cstdint>
#pragma warning(push)
#pragma warning(disable:4996)
#include <memory>
#pragma warning(pop)

namespace base {
namespace win {
	class _BASE_API file_version
	{
		const static WORD ansi_code_page = 1252;
	public:
		file_version();
		file_version(const wchar_t* module_path);
		file_version(HMODULE module_handle);
		const wchar_t* operator[] (const wchar_t* key) const;
		VS_FIXEDFILEINFO* fixed_file_info() const;

	protected:
		bool create(const wchar_t* module_path);
		bool create(HMODULE module_handle);
		bool get_value(WORD language, WORD code_page, const wchar_t* key, const wchar_t** value_ptr) const;

	protected:
		struct TRANSLATION {
			WORD language;
			WORD code_page;
		};

		TRANSLATION translation_;
		VS_FIXEDFILEINFO* fixed_file_info_;
#pragma warning(push)
#pragma warning(disable:4251)
		std::unique_ptr<uint8_t[]> version_info_;
#pragma warning(pop)
		bool vaild_;
	};

	class _BASE_API simple_file_version
	{
	public:
		simple_file_version();
		simple_file_version(const wchar_t* module_path, const wchar_t* key = L"FileVersion", const wchar_t pred = L',');
		simple_file_version(HMODULE module_handle, const wchar_t* key = L"FileVersion", const wchar_t pred = L',');

		uint32_t major;
		uint32_t minor;
		uint32_t revision;
		uint32_t build;
		uint32_t size;
	};
}
}
