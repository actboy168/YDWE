#pragma once

#include <base/win/registry/traits.h>
#include <base/win/registry/value.h>
#include <base/win/registry/exception.h>
#include <base/util/dynarray.h>
#include <boost/detail/scoped_enum_emulation.hpp>

namespace base { namespace registry {

BOOST_SCOPED_ENUM_START(open_option)
{
	none, 
	fail_if_exists = none, 
	create_if_exists,
};
BOOST_SCOPED_ENUM_END

template <typename C, typename T, typename V>
class basic_base_key
{
public:
	typedef C                                 char_type;
	typedef T                                 traits_type;
	typedef V                                 value_type;
	typedef basic_base_key<C, T, V>           class_type;
	typedef typename traits_type::size_type   size_type;
	typedef typename traits_type::string_type string_type;
	typedef typename traits_type::hkey_type   hkey_type;
	typedef typename traits_type::result_type result_type;
	typedef std::map<string_type, value_type> value_map_type;

public:
	basic_base_key                     ();
	basic_base_key                     (hkey_type hkeyParent, char_type const* keyName, REGSAM accessMask, BOOST_SCOPED_ENUM(open_option) option);
	basic_base_key                     (class_type const& keyParent, char_type const* keyName, REGSAM accessMask, BOOST_SCOPED_ENUM(open_option) option);
	basic_base_key                     (class_type const& rhs);
	basic_base_key                     (class_type const& rhs, REGSAM accessMask);
	basic_base_key                     (hkey_type* hkey, string_type const& keyName, REGSAM accessMask);
	virtual ~basic_base_key            () throw();
	class_type&         operator =     (class_type const& rhs);
	void                swap           (class_type& rhs) throw();

public:
	string_type const&  name           () const;
	hkey_type           handle         () const;
	REGSAM              access_mask    () const;
	value_type&         value          (const string_type& valueName);
	value_type&         operator[]     (const string_type& valueName);

protected:
	void                initialize_    (hkey_type hkeyParent, char_type const* keyName, REGSAM accessMask, BOOST_SCOPED_ENUM(open_option) option);

protected:							   
	static hkey_type    open_key_      (hkey_type hkeyParent, char_type const* keyName, REGSAM accessMask, BOOST_SCOPED_ENUM(open_option) option);
	static hkey_type    dup_key_       (hkey_type hkey, REGSAM accessMask);

protected:
	string_type    m_name;
	hkey_type      m_hkey;
	REGSAM         m_accessMask;
	value_map_type m_valueMap;
};

template <typename C, typename T, typename V>
inline basic_base_key<C, T, V>::basic_base_key()
	: m_name()
	, m_hkey(NULL)
	, m_accessMask(KEY_READ)
{ }

template <typename C, typename T, typename V>
inline basic_base_key<C, T, V>::basic_base_key(hkey_type hkeyParent, char_type const* keyName, REGSAM accessMask, BOOST_SCOPED_ENUM(open_option) option)
	: m_name()
	, m_hkey(NULL)
	, m_accessMask(KEY_READ)
{
	initialize_(hkeyParent, keyName, accessMask, option);
}

template <typename C, typename T, typename V>
inline basic_base_key<C, T, V>::basic_base_key(class_type const& keyParent, char_type const* keyName, REGSAM accessMask, BOOST_SCOPED_ENUM(open_option) option)
	: m_name()
	, m_hkey(NULL)
	, m_accessMask(KEY_READ)
{
	initialize_(keyParent.handle(), keyName, accessMask, option);
}

template <typename C, typename T, typename V>
inline basic_base_key<C, T, V>::basic_base_key(hkey_type* hkey, string_type const& keyName, REGSAM accessMask)
	: m_name(keyName)
	, m_hkey(*hkey)
	, m_accessMask(accessMask)
	, m_valueMap()
{ }

template <typename C, typename T, typename V>
inline basic_base_key<C, T, V>::basic_base_key(class_type const& rhs)
	: m_name(rhs.m_name)
	, m_hkey(dup_key_(rhs.m_hkey, rhs.access_mask()))
	, m_accessMask(rhs.m_accessMask)
	, m_valueMap()
{ }

template <typename C, typename T, typename V>
inline basic_base_key<C, T, V>::basic_base_key(class_type const& rhs, REGSAM accessMask)
	: m_name(rhs.m_name)
	, m_hkey(dup_key_(rhs.m_hkey, accessMask))
	, m_accessMask(accessMask)
	, m_valueMap()
{ }

template <typename C, typename T, typename V>
inline basic_base_key<C, T, V>::~basic_base_key() throw()
{
	if (m_hkey != NULL)
	{
		::RegCloseKey(m_hkey);
	}
}

template <typename C, typename T, typename V>
inline void basic_base_key<C, T, V>::swap(class_type& rhs) throw()
{
	std::swap(m_name,        rhs.m_name);
	std::swap(m_hkey,        rhs.m_hkey);
	std::swap(m_accessMask,  rhs.m_accessMask);
	std::swap(m_valueMap,    rhs.m_valueMap);
}

template <typename C, typename T, typename V>
inline void basic_base_key<C, T, V>::initialize_(hkey_type hkeyParent, char_type const* keyName, REGSAM accessMask, BOOST_SCOPED_ENUM(open_option) option)
{
	m_hkey       = open_key_(hkeyParent, keyName, accessMask, option);
	m_name       = keyName;
	m_accessMask = accessMask;
}

template <typename C, typename T, typename V>
inline typename basic_base_key<C, T, V>::hkey_type basic_base_key<C, T, V>::open_key_(hkey_type hkeyParent, char_type const* keyName, REGSAM accessMask, BOOST_SCOPED_ENUM(open_option) option)
{
	if (option == open_option::fail_if_exists)
	{
		hkey_type   hkey;
		result_type res = traits_type::open_key(hkeyParent, keyName, &hkey, accessMask);
		check_and_throw_exception("could not open key", res);
		return hkey;
	}
	else
	{
		assert(option == open_option::create_if_exists);

		static const char_type  s_emptyString[] = { '\0' };
		hkey_type hbasekey;
		hkey_type hkey;
		result_type res = traits_type::open_key(hkeyParent, s_emptyString, &hbasekey, KEY_CREATE_SUB_KEY);
		check_and_throw_exception("could not open key", res);
		res = traits_type::create_key(hbasekey, keyName, &hkey, accessMask);
		check_and_throw_exception("could not create sub-key", res);
		return hkey;
	}
}

template <typename C, typename T, typename V>
inline typename basic_base_key<C, T, V>::hkey_type basic_base_key<C, T, V>::dup_key_(hkey_type hkey, REGSAM accessMask)
{
	if (NULL == hkey)
	{
		return NULL;
	}

	result_type res;
	hkey_type   hkeyDup = traits_type::dup_key(hkey, accessMask, &res);
	check_and_throw_exception("could not duplicate key", res);
	return hkeyDup;
}

template <typename C, typename T, typename V>
inline typename basic_base_key<C, T, V>::class_type& basic_base_key<C, T, V>::operator =(class_type const& rhs)
{
	class_type  _this(rhs);
	swap(_this);
	return *this;
}

template <typename C, typename T, typename V>
inline typename basic_base_key<C, T, V>::string_type const& basic_base_key<C, T, V>::name() const
{
	return m_name;
}

template <typename C, typename T, typename V>
inline typename basic_base_key<C, T, V>::hkey_type basic_base_key<C, T, V>::handle() const
{
	return m_hkey;
}

template <typename C, typename T, typename V>
inline REGSAM basic_base_key<C, T, V>::access_mask() const
{
	return m_accessMask;
}

template <typename C, typename T, typename V>
inline typename basic_base_key<C, T, V>::value_type& basic_base_key<C, T, V>::value(const string_type& valueName)
{
	auto it = m_valueMap.find(valueName);
	if (it == m_valueMap.end())
	{
		m_valueMap.insert(std::make_pair(valueName, value_type(m_hkey, valueName)));
	}

	return m_valueMap[valueName];
}

template <typename C, typename T, typename V>
inline typename basic_base_key<C, T, V>::value_type& basic_base_key<C, T, V>::operator[](const string_type& valueName)
{
	return value(valueName);
}

template <typename C, typename T = reg_traits<C>, typename V = basic_read_value<C, T>>
class basic_read_key
	: public basic_base_key<C, T, V>
{
public:
	typedef C                                 char_type;
	typedef T                                 traits_type;
	typedef V                                 value_type;
	typedef basic_base_key<C, T, V>           base_type;
	typedef basic_read_key<C, T, V>           class_type;
	typedef typename traits_type::size_type   size_type;
	typedef typename traits_type::string_type string_type;
	typedef typename traits_type::hkey_type   hkey_type;
	typedef typename traits_type::result_type result_type;
	typedef std::map<string_type, value_type> value_map_type;

public:
	basic_read_key                     ();
	basic_read_key                     (hkey_type hkeyParent, char_type const* keyName);                          
	basic_read_key                     (class_type const& keyParent, char_type const* keyName);
	basic_read_key                     (class_type const& rhs);
	basic_read_key                     (hkey_type* hkey, string_type const& keyName);

public:
	string_type         reg_class      () const;
	size_type           num_sub_keys   () const;
	size_type           num_values     () const;

public:
	class_type          open_sub_key   (char_type const* subKeyName, REGSAM accessMask = KEY_ALL_ACCESS);
	bool                has_sub_key    (char_type const* subKeyName);
};

template <typename C, typename T, typename V>
inline basic_read_key<C, T, V>::basic_read_key()
	: base_type()
{ }

template <typename C, typename T, typename V>
inline basic_read_key<C, T, V>::basic_read_key(hkey_type hkeyParent, char_type const* keyName)
	: base_type()
{
	base_type::initialize_(hkeyParent, keyName, KEY_READ, open_option::fail_if_exists);
}

template <typename C, typename T, typename V>
inline basic_read_key<C, T, V>::basic_read_key(class_type const& keyParent, char_type const* keyName)
	: base_type()
{
	base_type::initialize_(keyParent.handle(), keyName, KEY_READ, open_option::fail_if_exists);
}

template <typename C, typename T, typename V>
inline basic_read_key<C, T, V>::basic_read_key(hkey_type* hkey, string_type const& keyName)
	: base_type(hkey, keyName)
{ }

template <typename C, typename T, typename V>
inline basic_read_key<C, T, V>::basic_read_key(class_type const& rhs)
	: base_type(rhs)
{ }

template <typename C, typename T, typename V>
inline typename basic_read_key<C, T, V>::string_type basic_read_key<C, T, V>::reg_class() const
{
	size_type   cch_key_class   =   0;
	result_type res             =   traits_type::query_info(m_hkey, NULL, &cch_key_class, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	check_and_throw_exception("could not determine the key registry class", res);
	std::dynarray<char_type> p(++cch_key_class);
	res = traits_type::query_info(m_hkey, p.data(), &cch_key_class, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	check_and_throw_exception("could not determine the key registry class", res);
	return string_type(p.data(), cch_key_class);
}

template <typename C, typename T, typename V>
inline typename basic_read_key<C, T, V>::size_type basic_read_key<C, T, V>::num_sub_keys() const
{
	uint32_t c_sub_keys;
	result_type res         =   traits_type::query_info(m_hkey, NULL, NULL, &c_sub_keys, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	check_and_throw_exception("could not determine the number of sub-keys", res);
	return c_sub_keys;
}

template <typename C, typename T, typename V>
inline typename basic_read_key<C, T, V>::size_type basic_read_key<C, T, V>::num_values() const
{
	uint32_t c_values;
	result_type res         =   traits_type::query_info(m_hkey, NULL, NULL, NULL, NULL, NULL, &c_values, NULL, NULL, NULL, NULL);
	check_and_throw_exception("could not determine the number of values", res);
	return c_values;
}

template <typename C, typename T, typename V>
inline typename basic_read_key<C, T, V>::class_type basic_read_key<C, T, V>::open_sub_key(char_type const* subKeyName, REGSAM accessMask /* = KEY_ALL_ACCESS */)
{
	return class_type(m_hkey, subKeyName, accessMask);
}

template <typename C, typename T, typename V>
inline bool basic_read_key<C, T, V>::has_sub_key(char_type const* subKeyName)
{
	hkey_type   hkey;
	result_type res = traits_type::open_key(m_hkey, subKeyName, &hkey, KEY_READ);

	switch (res)
	{
	case ERROR_SUCCESS:
		::RegCloseKey(hkey);
	case ERROR_ACCESS_DENIED:
		return true;
	default:
		return false;
	}
}

template <typename C, typename T = reg_traits<C>, typename V = basic_write_value<C, T>>
class basic_write_key
	: public basic_read_key<C, T, V>
{
public:
	typedef C                                 char_type;
	typedef T                                 traits_type;
	typedef V                                 value_type;
	typedef basic_read_key<C, T, V>           base_type;
	typedef basic_write_key<C, T, V>          class_type;
	typedef typename traits_type::size_type   size_type;
	typedef typename traits_type::string_type string_type;
	typedef typename traits_type::hkey_type   hkey_type;
	typedef typename traits_type::result_type result_type;
	typedef std::map<string_type, value_type> value_map_type;

public:
	basic_write_key                    ();
	basic_write_key                    (hkey_type hkeyParent, char_type const* keyName);                          
	basic_write_key                    (class_type const& keyParent, char_type const* keyName);
	basic_write_key                    (class_type const& rhs);
	basic_write_key                    (hkey_type* hkey, string_type const& keyName);

public:
	class_type          create_sub_key (char_type const* subKeyName, REGSAM accessMask = KEY_ALL_ACCESS);
	bool                delete_sub_key (char_type const* subKeyName);
};

template <typename C, typename T, typename V>
inline basic_write_key<C, T, V>::basic_write_key()
	: base_type()
{ }

template <typename C, typename T, typename V>
inline basic_write_key<C, T, V>::basic_write_key(hkey_type hkeyParent, char_type const* keyName)
	: base_type()
{
	base_type::initialize_(hkeyParent, keyName, KEY_WRITE, open_option::create_if_exists);
}

template <typename C, typename T, typename V>
inline basic_write_key<C, T, V>::basic_write_key(class_type const& keyParent, char_type const* keyName)
	: base_type()
{
	base_type::initialize_(keyParent.handle(), keyName, KEY_WRITE, open_option::create_if_exists);
}

template <typename C, typename T, typename V>
inline basic_write_key<C, T, V>::basic_write_key(hkey_type* hkey, string_type const& keyName)
	: base_type(hkey, keyName)
{ }

template <typename C, typename T, typename V>
inline basic_write_key<C, T, V>::basic_write_key(class_type const& rhs)
	: base_type(rhs)
{ }

template <typename C, typename T, typename V>
inline typename basic_write_key<C, T, V>::class_type basic_write_key<C, T, V>::create_sub_key(char_type const* subKeyName, REGSAM accessMask)
{
	hkey_type hkey;
	result_type res = traits_type::create_key(m_hkey, subKeyName, &hkey, accessMask);
	check_and_throw_exception("could not create sub-key", res);
	return class_type(&hkey, subKeyName, accessMask);
}

template <typename C, typename T, typename V>
inline bool basic_write_key<C, T, V>::delete_sub_key(char_type const* subKeyName)
{
	result_type res = traits_type::delete_key(m_hkey, subKeyName);

	switch(res)
	{
	case ERROR_SUCCESS:
		return true;
	default:
		check_and_throw_exception("could not delete sub-key", res);
	case ERROR_FILE_NOT_FOUND:
		return false;
	}
}

typedef basic_read_key <char,    reg_traits<char>>    read_key_a;
typedef basic_read_key <wchar_t, reg_traits<wchar_t>> read_key_w;
typedef basic_write_key<char,    reg_traits<char>>    write_key_a;
typedef basic_write_key<wchar_t, reg_traits<wchar_t>> write_key_w;
typedef write_key_a                                   key_a;
typedef write_key_w                                   key_w;

}}
