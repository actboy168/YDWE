#pragma once

#include <Windows.h>
#include <string>
#include <assert.h>	
#include <vector>  	
#include <list>	
#include <deque>	
#include <base/util/dynarray.h>
#include <base/win/registry/traits.h> 
#include <base/win/registry/exception.h>

namespace base { namespace registry {	

	template <class char_type, class T>
	struct is_stringable { static const bool value = false; };

	template <class char_type> struct is_stringable<char_type, std::vector<char_type>>       { static const bool value = true; };
	template <class char_type> struct is_stringable<char_type, std::list<char_type>>         { static const bool value = true; };
	template <class char_type> struct is_stringable<char_type, std::deque<char_type>>        { static const bool value = true; };


	template <class Target, class Source>
	inline Target reg_dispatch(const Source& s)
	{
		return std::move(Target(std::begin(s), std::end(s)));
	}

	template <typename C, typename T, class K>
	class basic_value
	{
	public:
		typedef C                                                 char_type;
		typedef T                                                 traits_type;
		typedef K                                                 key_type;
		typedef basic_value<C, T, K>                              class_type;
		typedef typename traits_type::size_type                   size_type;
		typedef typename traits_type::string_type                 string_type;
		typedef typename traits_type::hkey_type                   hkey_type;
		typedef std::dynarray<uint8_t>                            blob_type;
		typedef typename traits_type::result_type                 result_type;

		basic_value(key_type& key, const string_type& name);
		~basic_value();

		basic_value(class_type& rhs);
		class_type& operator =(class_type& rhs);

		basic_value(class_type&& rhs);
		class_type& operator =(class_type&& rhs);

	public:
		uint32_t                type() const;
		string_type             name() const;
		bool                    has() const;

		string_type             get_string() const;
		uint32_t                get_uint32_t() const;
		uint64_t                get_uint64_t() const;
		blob_type               get_binary() const;

		template <typename Result>
		Result get(typename std::enable_if<
			is_stringable<char_type, typename std::decay<Result>::type>::value
			, int>::type* = 0) const
		{
			return std::move(reg_dispatch<Result>(get_string()));
		}

		template <class Result>
		Result get(typename std::enable_if<
			!is_stringable<char_type, typename std::decay<Result>::type>::value
			, int>::type* = 0) const
		{
			return get_<Result>();
		}

		template <typename Result> operator Result () const { return get<Result>(); }

	public:
		bool                    del();
		bool                    set_uint32_t(uint32_t value);
		bool                    set_uint64_t(uint64_t value);
		bool                    set(const char*    value);
		bool                    set(const wchar_t* value);
		bool                    set(const std::string&  value);
		bool                    set(const std::wstring& value);
		bool                    set(const char*    value, size_type length);
		bool                    set(const wchar_t* value, size_type length);
		bool                    set(void const* value, size_type length);
		bool                    set(uint32_t type, void const* value, size_type length);

		template <typename Source>
		bool set(Source value, typename std::enable_if<
			std::is_unsigned<Source>::value && (sizeof(Source) == sizeof(uint32_t))
			, int>::type* = 0)
		{
			return set_uint32_t(value);
		}

		template <typename Source>
		bool set(Source value, typename std::enable_if<
			std::is_unsigned<Source>::value && (sizeof(Source) == sizeof(uint64_t))
			, int>::type* = 0)
		{
			return set_uint64_t(value);
		}

		template <typename Source>
		bool set(Source value, typename std::enable_if<
			is_stringable<char_type, typename std::decay<Source>::type>::value
			, int>::type* = 0)
		{
			return set(reg_dispatch<string_type>(value));
		}

		template <typename Source> class_type& operator =(Source s) { set(s); return *this; }

	protected:
		template <class R> R    get_() const;
		template <> string_type get_<string_type>() const { return get_string(); }
		template <> uint32_t    get_<uint32_t>() const { return get_uint32_t(); }
		template <> uint64_t    get_<uint64_t>() const { return get_uint64_t(); }
		template <> blob_type   get_<blob_type>() const { return get_binary(); }

	protected:
		key_type&        m_key;
		string_type      m_name;
		mutable uint32_t m_type;
		mutable bool     m_bTypeRetrieved;
	};


	template <typename C, typename T, typename K>
	inline basic_value<C, T, K>::basic_value(key_type& key, const string_type& name)
		: m_key(key)
		, m_name(name)
		, m_type(REG_NONE)
		, m_bTypeRetrieved(false)
	{ }

	template <typename C, typename T, typename K>
	inline basic_value<C, T, K>::basic_value(class_type&& rhs)
		: m_key(rhs.m_key)
		, m_name(rhs.m_name)
		, m_type(rhs.m_type)
		, m_bTypeRetrieved(rhs.m_bTypeRetrieved)
	{ }

	template <typename C, typename T, typename K>
	inline basic_value<C, T, K>::~basic_value() throw()
	{ }

	template <typename C, typename T, typename K>
	inline typename basic_value<C, T, K>::class_type& basic_value<C, T, K>::operator =(class_type&& rhs)
	{
		m_key = rhs.m_key;
		m_name = rhs.m_name;
		m_type = rhs.m_type;
		m_bTypeRetrieved = rhs.m_bTypeRetrieved;
		return *this;
	}

	template <typename C, typename T, typename K>
	inline uint32_t basic_value<C, T, K>::type() const
	{
		if (!m_bTypeRetrieved)
		{
			size_type   data_size = 0;
			if (0 == traits_type::query_value(m_key.handle(open_access::read), m_name.c_str(), m_type, NULL, data_size))
			{
				m_bTypeRetrieved = true;
			}
		}

		return m_type;
	}

	template <typename C, typename T, typename K>
	inline typename basic_value<C, T, K>::string_type basic_value<C, T, K>::name() const
	{
		return m_name;
	}

	template <typename C, typename T, typename K>
	inline typename basic_value<C, T, K>::string_type basic_value<C, T, K>::get_string() const
	{
		string_type ret;
		size_type   data_size = 0;
		uint32_t    dw;
		result_type res = traits_type::query_value(m_key.handle(open_access::read), m_name.c_str(), dw, NULL, data_size);
		check_and_throw_exception("could not determine the data size", res);

		if (data_size > 0)
		{
			std::dynarray<char_type> buffer(1 + data_size / sizeof(char_type));

			data_size = buffer.size() * sizeof(char_type);
			res = traits_type::query_value(m_key.handle(open_access::read), m_name.c_str(), dw, buffer.data(), data_size);
			check_and_throw_exception("could not elicit string value", res);

			if (data_size > 0)
			{
				assert(0 != data_size);
				data_size -= sizeof(char_type);
				buffer[data_size / sizeof(char_type)] = 0;
				ret.assign(buffer.data(), data_size / sizeof(char_type));

				if (ret.length() > 0 && REG_EXPAND_SZ == type())
				{
					size_type size = traits_type::expand_environment_strings(ret.c_str(), NULL, 0);

					if (0 != size)
					{
						std::dynarray<char_type> buffer(1 + size);

						if (0 == traits_type::expand_environment_strings(ret.c_str(), &buffer[0], size))
						{
							check_and_throw_exception("could not expand environment strings", ::GetLastError());
						}
						else
						{
							ret.assign(buffer.data(), size);
						}
					}
				}
			}
		}


		return ret;
	}

	template <typename C, typename T, typename K>
	inline uint32_t basic_value<C, T, K>::get_uint32_t() const
	{
		uint32_t  dwValue;
		size_type   cbData = sizeof(dwValue);
		uint32_t  value_type;
		result_type   res = traits_type::query_value(m_key.handle(open_access::read), m_name.c_str(), value_type, &dwValue, cbData);
		check_and_throw_exception("could not query value", res);
		return dwValue;
	}

	template <typename C, typename T, typename K>
	inline uint64_t basic_value<C, T, K>::get_uint64_t() const
	{
		uint64_t  dwValue;
		size_type   cbData = sizeof(dwValue);
		uint32_t  value_type;
		result_type   res = traits_type::query_value(m_key.handle(open_access::read), m_name.c_str(), value_type, &dwValue, cbData);
		check_and_throw_exception("could not query value", res);
		return dwValue;
	}

	template <typename C, typename T, typename K>
	inline typename basic_value<C, T, K>::blob_type basic_value<C, T, K>::get_binary() const
	{
		size_type   data_size = 0;
		uint32_t  dw;
		result_type   res = traits_type::query_value(m_key.handle(open_access::read), m_name.c_str(), dw, NULL, data_size);
		check_and_throw_exception("could not elicit binary value", res);
	
		assert(dw == REG_BINARY);
	
		if (data_size > 0)
		{
			blob_type buffer(data_size);
			data_size = buffer.size();
			res = traits_type::query_value(m_key.handle(open_access::read), m_name.c_str(), dw, buffer.data(), data_size);
			check_and_throw_exception("could not elicit binary value", res);
			return buffer;
		}
	
		return blob_type(0);
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::has() const
	{
		uint32_t    valueType;
		uint8_t     data[1];
		size_type   cbData = sizeof(data);
		result_type res = traits_type::query_value(m_key.handle(open_access::read), m_name.c_str(), valueType, &data[0], cbData);

		switch (res)
		{
		case ERROR_SUCCESS:
		case ERROR_MORE_DATA:
			return true;
		default:
			return false;
		}
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::set_uint32_t(uint32_t value)
	{
		return set(REG_DWORD, &value, sizeof(value));
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::set_uint64_t(uint64_t value)
	{
#ifndef REG_QWORD
		const DWORD REG_QWORD = 11;
#endif
		return set(REG_QWORD, &value, sizeof(value));
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::set(const char* value)
	{
		return set(value, std::string::traits_type::length(value));
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::set(const wchar_t* value)
	{
		return set(value, std::wstring::traits_type::length(value));
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::set(const std::string& value)
	{
		return set(value.c_str(), value.size());
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::set(const std::wstring& value)
	{
		return set(value.c_str(), value.size());
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::set(const char* value, size_type length)
	{
		return set(REG_SZ, value, length * sizeof(char));
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::set(const wchar_t* value, size_type length)
	{
		return set(REG_SZ, value, length * sizeof(wchar_t));
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::set(void const* value, size_type length)
	{
		return set(REG_BINARY, value, length);
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::set(uint32_t type, void const* value, size_type length)
	{
		result_type res = traits_type::set_value(m_key.handle(open_access::write), m_name.c_str(), type, value, length);
		check_and_throw_exception("could not create value", res);
		return ERROR_SUCCESS == res;
	}

	template <typename C, typename T, typename K>
	inline bool basic_value<C, T, K>::del()
	{
		result_type res = traits_type::delete_value(m_key.handle(open_access::write), m_name.c_str());

		switch (res)
		{
		case ERROR_SUCCESS:
			return true;
		default:
			check_and_throw_exception("could not delete value", res);
		case ERROR_FILE_NOT_FOUND:
			return false;
		}
	}
}}
