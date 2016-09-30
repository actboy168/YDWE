#pragma once

#include <Windows.h>
#include <string>
#include <map>	
#include <memory>
#include <assert.h>	
#include <base/win/registry/traits.h>   
#include <base/win/registry/value.h>

namespace base { namespace registry {

	namespace open_option 
	{
		enum  open_option
		{
			none,
			fail_if_not_exists = none,
			create_if_not_exists,
		};					  
		typedef open_option t;
	}

	namespace open_access 
	{
		enum open_access
		{
			none = 0,
			read = KEY_READ,
			write = KEY_READ | KEY_WRITE,
			w32key = KEY_WOW64_32KEY,
			w64key = KEY_WOW64_64KEY,
		};
		typedef open_access t;
	}

	template <typename C, typename T = reg_traits<C>>
	class basic_key
	{
	public:
		typedef C                                                  char_type;
		typedef T                                                  traits_type;
		typedef basic_key<C, T>                                    class_type;
		typedef basic_value<C, T, class_type>                      value_type;
		typedef typename traits_type::size_type                    size_type;
		typedef typename traits_type::string_type                  string_type;
		typedef typename traits_type::hkey_type                    hkey_type;
		typedef typename traits_type::result_type                  result_type;
		typedef std::map<string_type, std::unique_ptr<value_type>> value_map_type;

		basic_key(hkey_type keybase, open_access::t accessfix = open_access::none)
			: m_keybase(keybase)
			, m_keypath()
			, m_keyname()
			, m_key(NULL)
			, m_access(open_access::read)
			, m_accessfix(accessfix)
			, m_valuemap()
		{ }

		basic_key(hkey_type keybase, const string_type& keypath, const string_type& keyname, open_access::t accessfix = open_access::none)
			: m_keybase(keybase)
			, m_keypath(keypath)
			, m_keyname(keyname)
			, m_key(NULL)
			, m_access(open_access::read)
			, m_accessfix(accessfix)
			, m_valuemap()
		{ }

		basic_key(class_type const& rhs)
			: m_keybase(rhs.m_keybase)
			, m_keypath(rhs.m_keypath)
			, m_keyname(rhs.m_keyname)
			, m_key(rhs.m_key)
			, m_access(rhs.m_access)
			, m_accessfix(rhs.m_accessfix)
			, m_valuemap()
		{ }

		~basic_key() throw()
		{
			if (m_key != NULL)
			{
				traits_type::close(m_key);
			}
		}

		class_type& operator=(class_type const& rhs)
		{
			class_type  _this(rhs);
			swap(_this);
			return *this;
		}

		void swap(class_type& rhs) throw()
		{
			std::swap(m_keybase, rhs.m_keybase);
			std::swap(m_keypath, rhs.m_keypath);
			std::swap(m_keyname, rhs.m_keyname);
			std::swap(m_key, rhs.m_key);
			std::swap(m_access, rhs.m_access);
			std::swap(m_accessfix, rhs.m_accessfix);
			std::swap(m_valuemap, rhs.m_valuemap);
		}

		class_type sub_key(const string_type& sub_key_name) const
		{
			static const char_type s_separator[] = { '\\' };
			if (!m_keypath.empty())
			{
				return class_type(m_keybase, m_keypath + s_separator + m_keyname, sub_key_name, m_accessfix);
			}
			else if (!m_keyname.empty())
			{
				return class_type(m_keybase, m_keyname, sub_key_name, m_accessfix);
			}
			else
			{
				return class_type(m_keybase, string_type(), sub_key_name, m_accessfix);
			}
		}

		value_type& value(const string_type& value_name)
		{
			auto it = m_valuemap.find(value_name);
			if (it == m_valuemap.end())
			{
				m_valuemap.insert(std::make_pair(value_name, std::unique_ptr<value_type>(new value_type(*this, value_name))));
			}
			return *(m_valuemap[value_name].get());
		}

		value_type& operator[](const string_type& value_name)
		{
			return value(value_name);
		}

		hkey_type handle(open_access::t access)
		{
			open_key_(access);
			return m_key;
		}

		bool del(const string_type& subkey_name, bool delete_tree)
		{
			result_type res = delete_tree
				? traits_type::delete_tree(handle(open_access::write), subkey_name.c_str())
				: traits_type::delete_key(handle(open_access::write), subkey_name.c_str());
			switch (res)
			{
			case ERROR_SUCCESS:
				return true;
			default:
				check_and_throw_exception("could not delete sub-key", res);
			case ERROR_FILE_NOT_FOUND:
				return false;
			}
		}

		bool del()
		{
			hkey_type key = open_key_(m_keybase, m_keypath.c_str(), open_access::write | m_accessfix, open_option::fail_if_not_exists);
			if (key == NULL)
			{
				return false;
			}
			result_type res = traits_type::delete_tree(key, m_keyname.c_str());
			bool suc = false;
			switch (res)
			{
			case ERROR_SUCCESS:
				suc = true;
				break;
			default:
			case ERROR_FILE_NOT_FOUND:
				suc = false;
				break;
			}
			traits_type::close(key);
			return suc;
		}

	protected:
		bool open_key_(open_access::t access)
		{
			hkey_type key = NULL;
			if (access == open_access::write)
			{
				open_option::t option = open_option::create_if_not_exists;
				if (m_key)
				{
					if (m_access == open_access::write)
					{
						key = m_key;
					}
					else
					{
						close_key_();
						key = open_key_(m_keybase, key_name_(), access | m_accessfix, option);
					}
				}
				else
				{
					key = open_key_(m_keybase, key_name_(), access | m_accessfix, option);
				}
			}
			else
			{
				assert(access == open_access::read);
				open_option::t option = open_option::fail_if_not_exists;
				if (m_key)
				{
					key = m_key;
				}
				else
				{
					key = open_key_(m_keybase, key_name_(), access | m_accessfix, option);
				}
			}

			if (!key) 
				return false;
			m_key = key;
			m_access = access;
			return true;
		}

		string_type key_name_()
		{
			static const char_type s_separator[] = { '\\' };
			return m_keypath.empty() ? m_keyname : m_keypath + s_separator + m_keyname;
		}

		void close_key_()
		{
			traits_type::close(m_key);
			m_key = NULL;
			m_access = open_access::read;
		}

		static hkey_type open_key_(hkey_type key_parent, const string_type& key_name, REGSAM access_mask, open_option::t option)
		{
			if (option == open_option::fail_if_not_exists)
			{
				hkey_type   hkey;
				result_type res = traits_type::open_key(key_parent, key_name.c_str(), &hkey, access_mask);
				check_and_throw_exception("could not open key", res);
				return hkey;
			}
			else
			{
				assert(option == open_option::create_if_not_exists);
				static const char_type  s_empty_string[] = { '\0' };
				hkey_type hbasekey;
				hkey_type hkey;
				result_type res = traits_type::open_key(key_parent, s_empty_string, &hbasekey, KEY_CREATE_SUB_KEY);
				check_and_throw_exception("could not open key", res);
				res = traits_type::create_key(hbasekey, key_name.c_str(), &hkey, access_mask);
				check_and_throw_exception("could not create sub-key", res);
				return hkey;
			}
		}

		static hkey_type dup_key_(hkey_type hkey, open_access::t access_mask)
		{
			if (NULL == hkey) return NULL;
			result_type res;
			hkey_type   hkey_dup = traits_type::dup_key(hkey, access_mask, &res);
			check_and_throw_exception("could not duplicate key", res);
			return hkey_dup;
		}

	protected:
		hkey_type      m_keybase;
		string_type    m_keypath;
		string_type    m_keyname;
		hkey_type      m_key;
		open_access::t m_access;
		open_access::t m_accessfix;
		value_map_type m_valuemap;
	};

	template <typename C, typename T>
	inline basic_key<C, T> operator/(const basic_key<C, T>& lhs, const typename basic_key<C, T>::string_type& rhs)
	{
		return lhs.sub_key(rhs);
	}

	typedef basic_key<char>    key_a;
	typedef basic_key<wchar_t> key_w;
}}

#include <base/win/registry/predefined_keys.h>
