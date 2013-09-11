#include <base/win/file_version.h>

#include <boost/algorithm/string.hpp>
#include <base/util/format.h>
#include <vector>

#pragma comment(lib, "version.lib")

namespace base { namespace win {

	file_version::file_version() 
		: fixed_file_info_(nullptr)
		, vaild_(false)
	{ }

	file_version::file_version(const wchar_t* module_path)
		: version_info_()
		, vaild_(create(module_path))
	{ }

	const wchar_t* file_version::operator[] (const wchar_t* key) const
	{
		if (!vaild_)
			return L"";

		try
		{
			const wchar_t* value = nullptr;
			if (get_value(translation_.language, translation_.code_page ,key, &value))
			{
				return value;
			}

			if (get_value(::GetUserDefaultLangID(), translation_.code_page ,key, &value))
			{
				return value;
			}

			if (get_value(translation_.language, ansi_code_page ,key, &value))
			{
				return value;
			}

			if (get_value(::GetUserDefaultLangID(), ansi_code_page ,key, &value))
			{
				return value;
			}
		}
		catch (...)
		{
			assert(false);
		}

		return L"";
	}

	VS_FIXEDFILEINFO* file_version::fixed_file_info() const
	{
		return fixed_file_info_; 
	}

	bool file_version::create(const wchar_t* module_path)
	{
		DWORD dummy_handle;
		DWORD size = ::GetFileVersionInfoSizeW(module_path, &dummy_handle);
		if (size <= 0)
			return false;

		version_info_.reset(new uint8_t[size]);
		if (!::GetFileVersionInfoW(module_path, dummy_handle, size, version_info_.get()))
			return false;

		UINT length;
		if (!::VerQueryValueW(version_info_.get(), L"\\", (LPVOID*)&fixed_file_info_, &length))
			return false;

		TRANSLATION* translate_ptr = nullptr;
		if (::VerQueryValueW(version_info_.get(), L"\\VarFileInfo\\Translation", (LPVOID*)&translate_ptr, &length) 
			&& (length >= sizeof TRANSLATION))
		{
			translation_ = *(TRANSLATION*)translate_ptr;
		}
		else
		{
			translation_.language  = ::GetUserDefaultLangID();
			translation_.code_page = ansi_code_page;
		}

		return fixed_file_info()->dwSignature == VS_FFI_SIGNATURE;
	}

	bool file_version::get_value(WORD language, WORD code_page, const wchar_t* key, const wchar_t** value_ptr) const
	{
		assert(value_ptr);
		UINT size;
		std::wstring query = util::format(L"\\StringFileInfo\\%04x%04x\\%s", language, code_page, key);
		return (!!::VerQueryValueW(version_info_.get(), (LPWSTR)(LPCWSTR)query.c_str(), (LPVOID*)value_ptr, &size));
	}

	int stoi_no_throw(std::wstring const& str)
	{
		try {
			return std::stoi(str);
		}
		catch (...) {
		}

		return 0;
	}

	simple_file_version::simple_file_version()
		: major(0)
		, minor(0)
		, revision(0)
		, build(0)
	{ }

	simple_file_version::simple_file_version(const wchar_t* module_path)
	{
		std::wstring version_string = file_version(module_path)[L"FileVersion"];
		std::vector<std::wstring> version_array;
		boost::algorithm::split(version_array, version_string, boost::algorithm::is_any_of(L","));
		major       = (version_array.size() > 0) ? stoi_no_throw(version_array[0]) : 0;
		minor       = (version_array.size() > 1) ? stoi_no_throw(version_array[1]) : 0;
		revision    = (version_array.size() > 2) ? stoi_no_throw(version_array[2]) : 0;
		build       = (version_array.size() > 3) ? stoi_no_throw(version_array[3]) : 0;
	}
}}
