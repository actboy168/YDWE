#pragma once

#include <cassert>
#include <cstdint> 
#include <string>
#include <Windows.h>

namespace base { namespace registry {

	template <class C>
	struct reg_traits
	{
	public:
		typedef C                    char_type;
		typedef size_t               size_type;
		typedef ptrdiff_t            difference_type;
		typedef HKEY                 hkey_type;
		typedef std::basic_string<C> string_type;
		typedef FILETIME             time_type;
		typedef LONG                 result_type;

	public:
		static result_type close       (hkey_type hkey);
		static hkey_type   dup_key     (hkey_type hkey, REGSAM samDesired = KEY_ALL_ACCESS, result_type* result = NULL);
		static result_type open_key    (hkey_type hkey, char_type const* sub_key_name, hkey_type* hkey_result, REGSAM samDesired = KEY_ALL_ACCESS);
		static result_type create_key  (hkey_type hkey, char_type const* sub_key_name, hkey_type* hkey_result, REGSAM samDesired = KEY_ALL_ACCESS);
		static result_type create_key  (hkey_type hkey, char_type const* sub_key_name, hkey_type* hkey_result, bool& bCreated, REGSAM samDesired = KEY_ALL_ACCESS);		 
		static result_type delete_key  (hkey_type hkey, char_type const* sub_key_name);
		static result_type query_value (hkey_type hkey, char_type const* valueName, uint32_t& valueType, void* data, size_type& cbData);
		static result_type set_value   (hkey_type hkey, char_type const* valueName, uint32_t valueType, void const* data, size_type cbData);
		static result_type delete_value(hkey_type hkey, char_type const* valueName);
		static result_type delete_tree (hkey_type hkey, char_type const* sub_key_name);
		static result_type query_info  (hkey_type hkey, char_type* key_class, size_type* cch_key_class, uint32_t* c_sub_keys, size_type* cch_sub_key_max, size_type* cch_key_class_max, uint32_t* c_values, size_type* cch_valueName_max, size_type* cb_value_data_max, size_type* cb_security_descriptor_max, time_type* time_last_write);
		static result_type enum_key    (hkey_type hkey, uint32_t index, char_type* key_name, size_type* cch_key_name, time_type* time_last_write = NULL);
		static result_type enum_key    (hkey_type hkey, uint32_t index, char_type* key_name, size_type* cch_key_name, char_type* key_class, size_type* cch_key_class, time_type* time_last_write);
		static result_type enum_value  (hkey_type hkey, uint32_t index, char_type* valueName, size_type* cch_valueName, uint32_t* valueType, void* data, size_type& cbData);
		static result_type enum_value  (hkey_type hkey, uint32_t index, char_type* valueName, size_type* cch_valueName);
		static size_type   expand_environment_strings(char_type const* src, char_type* dest, size_type cch_dest);
	};

	template <>
	struct reg_traits<char>
	{
	public:
		typedef char            char_type;
		typedef size_t          size_type;
		typedef ptrdiff_t       difference_type;
		typedef HKEY            hkey_type;
		typedef std::string     string_type;
		typedef FILETIME        time_type;
		typedef LONG            result_type;

	public:
		static result_type close(hkey_type hkey)
		{
			return ::RegCloseKey(hkey);
		}

		static hkey_type dup_key(hkey_type hkey, REGSAM samDesired, result_type* result = NULL)
		{
			hkey_type   hkeyDup;
			result_type res = ::RegOpenKeyExA(hkey, "", 0, samDesired, &hkeyDup);

			if (ERROR_SUCCESS != res)
			{
				hkeyDup = NULL;
			}

			if (NULL != result)
			{
				*result = res;
			}

			return hkeyDup;
		}

		static result_type open_key(hkey_type hkey, char_type const* sub_key_name, hkey_type* hkey_result, REGSAM samDesired = KEY_ALL_ACCESS)
		{
			return ::RegOpenKeyExA(hkey, sub_key_name, 0, samDesired, hkey_result);
		}

		static result_type create_key(hkey_type hkey, char_type const* sub_key_name, hkey_type* hkey_result, REGSAM samDesired = KEY_ALL_ACCESS)
		{
			return ::RegCreateKeyExA(hkey, sub_key_name, 0, NULL, 0, samDesired, NULL, hkey_result, NULL);
		}

		static result_type create_key(hkey_type hkey, char_type const* sub_key_name, hkey_type* hkey_result, bool& bCreated, REGSAM samDesired = KEY_ALL_ACCESS)
		{
			DWORD       disposition;
			result_type res =   ::RegCreateKeyExA(hkey, sub_key_name, 0, NULL, 0, samDesired, NULL, hkey_result, &disposition);
			bCreated = (ERROR_SUCCESS == res) && (REG_CREATED_NEW_KEY == disposition);
			return res;
		}

		static result_type delete_key(hkey_type hkey, char_type const* sub_key_name)
		{
			return ::RegDeleteKeyA(hkey, sub_key_name);
		}

		static result_type query_value(hkey_type hkey, char_type const* valueName, uint32_t& valueType, void* data, size_type& cbData)
		{
			return ::RegQueryValueExA(hkey, valueName, NULL, reinterpret_cast<LPDWORD>(&valueType), static_cast<LPBYTE>(data), reinterpret_cast<LPDWORD>(&cbData));
		}

		static result_type set_value(hkey_type hkey, char_type const* valueName, uint32_t valueType, void const* data, size_type cbData)
		{
			return ::RegSetValueExA(hkey, valueName, 0, valueType, static_cast<BYTE const*>(data), static_cast<DWORD>(cbData));
		}

		static result_type delete_value(hkey_type hkey, char_type const* valueName)
		{
			return ::RegDeleteValueA(hkey, valueName);
		}

		static result_type delete_tree(hkey_type hkey, char_type const* sub_key_name)
		{
			result_type res = execute_dynamic_("advapi32.dll", "RegDeleteTreeA", hkey, sub_key_name);

			if (ERROR_PROC_NOT_FOUND == res)
			{
				res = execute_dynamic_("shlwapi.dll", "SHDeleteKeyA", hkey, sub_key_name);
			}

			return res;
		}

		static result_type query_info(hkey_type hkey,
			char_type*      key_class,
			size_type*      cch_key_class,
			uint32_t*       c_sub_keys,
			size_type*      cch_sub_key_max,
			size_type*      cch_key_class_max,
			uint32_t*       c_values,
			size_type*      cch_valueName_max,
			size_type*      cb_value_data_max,
			size_type*      cb_security_descriptor_max,
			time_type*      time_last_write)
		{
			if (NULL == cch_key_class && NULL != key_class)
			{
				return ERROR_INVALID_PARAMETER;
			}

			return ::RegQueryInfoKeyA(hkey, key_class, reinterpret_cast<LPDWORD>(cch_key_class), NULL, reinterpret_cast<LPDWORD>(c_sub_keys), reinterpret_cast<LPDWORD>(cch_sub_key_max), reinterpret_cast<LPDWORD>(cch_key_class_max), reinterpret_cast<LPDWORD>(c_values), reinterpret_cast<LPDWORD>(cch_valueName_max), reinterpret_cast<LPDWORD>(cb_value_data_max), reinterpret_cast<LPDWORD>(cb_security_descriptor_max), time_last_write);
		}

		static result_type enum_key(hkey_type       hkey,
			uint32_t      index,
			char_type*      key_name,
			size_type*      cch_key_name,
			time_type*      time_last_write    =   NULL)
		{
			return ::RegEnumKeyExA(hkey, index, key_name, reinterpret_cast<LPDWORD>(cch_key_name), NULL, NULL, NULL, time_last_write);
		}

		static result_type enum_key(hkey_type       hkey,
			uint32_t      index,
			char_type*      key_name,
			size_type*      cch_key_name,
			char_type*      key_class,
			size_type*      cch_key_class,
			time_type*      time_last_write)
		{
			return ::RegEnumKeyExA(hkey, index, key_name, reinterpret_cast<LPDWORD>(cch_key_name), NULL, key_class, reinterpret_cast<LPDWORD>(cch_key_class), time_last_write);
		}

		static result_type enum_value(hkey_type hkey, uint32_t index, char_type* valueName, size_type* cch_valueName, uint32_t* valueType, void* data, size_type&  cbData)
		{
			return ::RegEnumValueA(hkey, index, valueName, reinterpret_cast<LPDWORD>(cch_valueName), NULL, reinterpret_cast<LPDWORD>(valueType), reinterpret_cast<LPBYTE>(data), reinterpret_cast<LPDWORD>(&cbData));
		}

		static result_type enum_value(hkey_type hkey, uint32_t index, char_type* valueName, size_type* cch_valueName)
		{
			return ::RegEnumValueA(hkey, index, valueName, reinterpret_cast<LPDWORD>(cch_valueName), NULL, NULL, NULL, NULL);
		}

		static size_type   expand_environment_strings(char_type const* src, char_type* dest, size_type cch_dest)
		{
			assert(nullptr != src);
			assert(nullptr != dest || 0 == cch_dest);

			return ::ExpandEnvironmentStringsA(src, dest, cch_dest);
		}

	private:
		static result_type execute_dynamic_(const char*  module, const char* function, hkey_type a1, char_type const* a2)
		{
			result_type r       =   ERROR_SUCCESS;
			HINSTANCE   hinst   =   ::LoadLibraryA(module);

			if (NULL == hinst)
			{
				r = static_cast<result_type>(::GetLastError());
			}
			else
			{
				union
				{
					FARPROC                fp;
					DWORD (__stdcall* pfn)(HKEY, LPCSTR);
				} u;
				u.fp = ::GetProcAddress(hinst, function);

				if (NULL == u.fp)
				{
					r = static_cast<result_type>(::GetLastError());
				}

				else
				{
					r = static_cast<result_type>((*u.pfn)(a1, a2));
				}

				::FreeLibrary(hinst);
			}

			return r;
		}
	};

	template <>
	struct reg_traits<wchar_t>
	{
	public:
		typedef wchar_t        char_type;
		typedef size_t         size_type;
		typedef ptrdiff_t      difference_type;
		typedef HKEY           hkey_type;
		typedef std::wstring   string_type;
		typedef FILETIME       time_type;
		typedef LONG           result_type;

	public:
		static result_type close(hkey_type hkey)
		{
			return ::RegCloseKey(hkey);
		}

		static hkey_type dup_key(hkey_type hkey, REGSAM samDesired, result_type* result = NULL)
		{
			hkey_type   hkeyDup;
			result_type res = ::RegOpenKeyExW(hkey, L"", 0, samDesired, &hkeyDup);

			if (ERROR_SUCCESS != res)
			{
				hkeyDup = NULL;
			}

			if (NULL != result)
			{
				*result = res;
			}

			return hkeyDup;
		}

		static result_type open_key(hkey_type hkey, char_type const* sub_key_name, hkey_type* hkey_result, REGSAM samDesired = KEY_ALL_ACCESS)
		{
			return ::RegOpenKeyExW(hkey, sub_key_name, 0, samDesired, hkey_result);
		}

		static result_type create_key(hkey_type hkey, char_type const* sub_key_name, hkey_type* hkey_result, REGSAM samDesired = KEY_ALL_ACCESS)
		{
			return ::RegCreateKeyExW(hkey, sub_key_name, 0, NULL, 0, samDesired, NULL, hkey_result, NULL);
		}

		static result_type create_key(hkey_type hkey, char_type const* sub_key_name, hkey_type* hkey_result, bool& bCreated, REGSAM samDesired = KEY_ALL_ACCESS)
		{
			DWORD       disposition;
			result_type res =   ::RegCreateKeyExW(hkey, sub_key_name, 0, NULL, 0, samDesired, NULL, hkey_result, &disposition);
			bCreated = (ERROR_SUCCESS == res) && (REG_CREATED_NEW_KEY == disposition);
			return res;
		}

		static result_type delete_key(hkey_type hkey, char_type const* sub_key_name)
		{
			return ::RegDeleteKeyW(hkey, sub_key_name);
		}

		static result_type query_value(hkey_type hkey, char_type const* valueName, uint32_t& valueType, void* data, size_type& cbData)
		{
			return ::RegQueryValueExW(hkey, valueName, NULL, reinterpret_cast<LPDWORD>(&valueType), static_cast<LPBYTE>(data), reinterpret_cast<LPDWORD>(&cbData));
		}

		static result_type set_value(hkey_type hkey, char_type const* valueName, uint32_t valueType, void const* data, size_type cbData)
		{
			return ::RegSetValueExW(hkey, valueName, 0, valueType, static_cast<BYTE const*>(data), static_cast<DWORD>(cbData));
		}

		static result_type delete_value(hkey_type hkey, char_type const* valueName)
		{
			return ::RegDeleteValueW(hkey, valueName);
		}

		static result_type delete_tree(
			hkey_type           hkey
			,   char_type const*    sub_key_name
			)
		{
			result_type res = execute_dynamic_(L"advapi32.dll", "RegDeleteTreeW", hkey, sub_key_name);

			if (ERROR_PROC_NOT_FOUND == res)
			{
				res = execute_dynamic_(L"shlwapi.dll", "SHDeleteKeyW", hkey, sub_key_name);
			}

			return res;
		}

		static result_type query_info(hkey_type hkey,
			char_type*  key_class,
			size_type*  cch_key_class,
			uint32_t*   c_sub_keys,
			size_type*  cch_sub_key_max,
			size_type*  cch_key_class_max,
			uint32_t*   c_values,
			size_type*  cch_valueName_max,
			size_type*  cb_value_data_max,
			size_type*  cb_security_descriptor_max,
			time_type*  time_last_write)
		{
			if (NULL == cch_key_class && NULL != key_class)
			{
				return ERROR_INVALID_PARAMETER;
			}

			return ::RegQueryInfoKeyW(hkey, key_class, reinterpret_cast<LPDWORD>(cch_key_class), NULL, reinterpret_cast<LPDWORD>(c_sub_keys), reinterpret_cast<LPDWORD>(cch_sub_key_max), reinterpret_cast<LPDWORD>(cch_key_class_max), reinterpret_cast<LPDWORD>(c_values), reinterpret_cast<LPDWORD>(cch_valueName_max), reinterpret_cast<LPDWORD>(cb_value_data_max), reinterpret_cast<LPDWORD>(cb_security_descriptor_max), time_last_write);
		}

		static result_type enum_key(hkey_type       hkey,
			uint32_t     index,
			char_type*   key_name,
			size_type*   cch_key_name,
			time_type*   time_last_write    =   NULL)
		{
			return ::RegEnumKeyExW(hkey, index, key_name, reinterpret_cast<LPDWORD>(cch_key_name), NULL, NULL, NULL, time_last_write);
		}

		static result_type enum_key(hkey_type       hkey,
			uint32_t      index,
			char_type*    key_name,
			size_type*    cch_key_name,
			char_type*    key_class,
			size_type*    cch_key_class,
			time_type*    time_last_write)
		{
			return ::RegEnumKeyExW(hkey, index, key_name, reinterpret_cast<LPDWORD>(cch_key_name), NULL, key_class, reinterpret_cast<LPDWORD>(cch_key_class), time_last_write);
		}

		static result_type enum_value(hkey_type       hkey,
			uint32_t      index,
			char_type*    valueName,
			size_type*    cch_valueName,
			uint32_t*     valueType,
			void*         data,
			size_type&    cbData)
		{
			return ::RegEnumValueW(hkey, index, valueName, reinterpret_cast<LPDWORD>(cch_valueName), NULL, reinterpret_cast<LPDWORD>(valueType), reinterpret_cast<LPBYTE>(data), reinterpret_cast<LPDWORD>(&cbData));
		}

		static result_type enum_value(hkey_type hkey, uint32_t index, char_type* valueName, size_type* cch_valueName)
		{
			return ::RegEnumValueW(hkey, index, valueName, reinterpret_cast<LPDWORD>(cch_valueName), NULL, NULL, NULL, NULL);
		}

		static size_type   expand_environment_strings(char_type const* src, char_type* dest, size_type cch_dest)
		{
			assert(nullptr != src);
			assert(nullptr != dest || 0 == cch_dest);

			return ::ExpandEnvironmentStringsW(src, dest, cch_dest);
		}

	private:
		static result_type execute_dynamic_(const wchar_t*  module, const char* function, hkey_type a1, char_type const* a2)
		{
			result_type r       =   ERROR_SUCCESS;
			HINSTANCE   hinst   =   ::LoadLibraryW(module);

			if (NULL == hinst)
			{
				r = static_cast<result_type>(::GetLastError());
			}
			else
			{
				union
				{
					FARPROC               fp;
					DWORD (__stdcall* pfn)(HKEY, LPCWSTR);
				} u;
				u.fp = ::GetProcAddress(hinst, function);

				if (NULL == u.fp)
				{
					r = static_cast<result_type>(::GetLastError());
				}

				else
				{
					r = static_cast<result_type>((*u.pfn)(a1, a2));
				}

				::FreeLibrary(hinst);
			}

			return r;
		}

	};
}}
