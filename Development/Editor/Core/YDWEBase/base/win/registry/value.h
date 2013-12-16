#pragma once

#include <base/win/registry/traits.h>
#include <base/util/dynarray.h>
#include <base/util/noncopyable.h>
#include <type_traits>
#include <string>

namespace base { namespace registry {

	template<typename C, typename T = reg_traits<C>>
	class basic_read_value
		: private util::noncopyable
	{
	public:
		typedef C                                                 char_type;
		typedef T                                                 traits_type;
		typedef basic_read_value<C, T>                            class_type;
		typedef typename traits_type::size_type                   size_type;
		typedef typename traits_type::string_type                 string_type;
		typedef typename traits_type::hkey_type                   hkey_type;
		typedef std::dynarray<uint8_t>                            blob_type;
		typedef typename traits_type::result_type                 result_type;

	public:
		basic_read_value();
		basic_read_value(class_type&& rhs);
		basic_read_value(hkey_type hkeyParent, string_type const& value_name);
		~basic_read_value() throw();
		class_type& operator =(class_type&& rhs);

	public:
		uint32_t                type        () const;
		string_type             name        () const;
		bool                    has         () const;

		string_type             get_string  () const;
		uint32_t                get_uint32_t() const;
		uint64_t                get_uint64_t() const;
		blob_type               get_binary  () const;

		template <typename Result>
		Result get(typename std::enable_if<
			is_stringable<char_type, typename boost::decay<Result>::type>::value
			, int>::type* = 0) const 
		{
			return std::move(reg_dispatch<Result>(get_string()));
		}

		template <class Result> 
		Result get(typename std::enable_if<
			!is_stringable<char_type, typename boost::decay<Result>::type>::value
			, int>::type* = 0) const
		{
			return get_<Result>();
		}

	public:
		template <typename Result> operator Result () const { return get<Result>(); }

	private:
		template <class R> R    get_() const;
		template <> string_type get_<string_type>() const { return get_string(); }
		template <> uint32_t    get_<uint32_t>   () const { return get_uint32_t(); }
		template <> uint64_t    get_<uint64_t>   () const { return get_uint64_t(); }
		template <> blob_type   get_<blob_type>  () const { return get_binary(); }

	protected:
		string_type      m_name;
		hkey_type        m_hkey;
		mutable uint32_t m_type;
		mutable bool     m_bTypeRetrieved;
	};

	template <typename C, typename T>
	inline basic_read_value<C, T>::basic_read_value()
		: m_name()
		, m_hkey(NULL)
		, m_type(REG_NONE)
		, m_bTypeRetrieved(false)
	{ }

	template <typename C, typename T>
	inline basic_read_value<C, T>::basic_read_value(class_type&& rhs)
		: m_name(rhs.m_name)
		, m_hkey(rhs.m_hkey)
		, m_type(rhs.m_type)
		, m_bTypeRetrieved(rhs.m_bTypeRetrieved)
	{ }

	template <typename C, typename T>
	inline basic_read_value<C, T>::basic_read_value(hkey_type hkeyParent, string_type const& value_name)
		: m_name(value_name)
		, m_hkey(hkeyParent)
		, m_type(REG_NONE)
		, m_bTypeRetrieved(false)
	{ } 

	template <typename C, typename T>
	inline basic_read_value<C, T>::~basic_read_value() throw()
	{ }

	template <typename C, typename T>
	inline typename basic_read_value<C, T>::class_type& basic_read_value<C, T>::operator =(class_type&& rhs)
	{
		m_name           = rhs.m_name;
		m_type           = rhs.m_type;
		m_bTypeRetrieved = rhs.m_bTypeRetrieved;
		m_hkey           = rhs.m_hkey;
		return *this;
	}

	template <typename C, typename T>
	inline uint32_t basic_read_value<C, T>::type() const
	{
		if (!m_bTypeRetrieved)
		{
			size_type   data_size = 0;
			if (0 == traits_type::query_value(m_hkey, m_name.c_str(), m_type, NULL, data_size))
			{
				m_bTypeRetrieved = true;
			}
		}

		return m_type;
	}

	template <typename C, typename T>
	inline typename basic_read_value<C, T>::string_type basic_read_value<C, T>::name() const
	{
		return m_name;
	}

	template <typename C, typename T>
	inline typename basic_read_value<C, T>::string_type basic_read_value<C, T>::get_string() const
	{
		string_type ret;
		size_type   data_size = 0;
		result_type res       = traits_type::query_info(m_hkey, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &data_size, NULL, NULL);
		check_and_throw_exception("could not determine the data size", res);

		if (data_size > 0)
		{
			std::dynarray<char_type> buffer(1 + data_size);
			uint32_t               dw;

			data_size = buffer.size();
			res = traits_type::query_value(m_hkey, m_name.c_str(), dw, buffer.data(), data_size);
			check_and_throw_exception("could not elicit string value", res);

			if (data_size > 0)
			{
				assert(0 != data_size);
				--data_size;
				buffer[data_size / sizeof(char_type)] = 0;
				ret.assign(buffer.data(), data_size / sizeof(char_type));

				if (ret.length() > 0 && REG_EXPAND_SZ == type())
				{
					size_type size = traits_type::expand_environment_strings(ret.c_str(), NULL, 0);

					if(0 != size)
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

	template <typename C, typename T>
	inline uint32_t basic_read_value<C, T>::get_uint32_t() const
	{
		uint32_t  dwValue;
		size_type   cbData  =   sizeof(dwValue);
		uint32_t  value_type;
		result_type   res   =   traits_type::query_value(m_hkey, m_name.c_str(), value_type, &dwValue, cbData);
		check_and_throw_exception("could not query value", res);
		return dwValue;
	}

	template <typename C, typename T>
	inline uint64_t basic_read_value<C, T>::get_uint64_t() const
	{
		uint64_t  dwValue;
		size_type   cbData  =   sizeof(dwValue);
		uint32_t  value_type;
		result_type   res   =   traits_type::query_value(m_hkey, m_name.c_str(), value_type, &dwValue, cbData);
		check_and_throw_exception("could not query value", res);
		return dwValue;
	}

	template <typename C, typename T>
	inline typename basic_read_value<C, T>::blob_type basic_read_value<C, T>::get_binary() const
	{
		size_type   data_size   =   0;
		uint32_t  dw;
		result_type   res       =   traits_type::query_value(m_hkey, m_name.c_str(), dw, NULL, data_size);
		check_and_throw_exception("could not elicit binary value", res);

		assert(dw == REG_BINARY);

		if (data_size > 0)
		{
			std::dynarray<uint8_t> buffer(data_size);

			data_size = buffer.size();
			res = traits_type::query_value(m_hkey, m_name.c_str(), dw, buffer.data(), data_size);
			check_and_throw_exception("could not elicit binary value", res);
			return blob_type(buffer.data(), buffer.size());
		}

		return blob_type();
	}

	template <typename C, typename T>
	inline bool basic_read_value<C, T>::has() const
	{
		uint32_t    valueType;
		uint8_t     data[1];
		size_type   cbData = sizeof(data);
		result_type res = traits_type::query_value(m_hkey, m_name.c_str(), valueType, &data[0], cbData);

		switch (res)
		{
		case ERROR_SUCCESS:
		case ERROR_MORE_DATA:
			return true;
		default:
			return false;
		}
	}

	template<typename C, typename T = reg_traits<C>>
	class basic_write_value
		: public basic_read_value<C, T>
	{
	public:
		typedef C                                                 char_type;
		typedef T                                                 traits_type;
		typedef basic_read_value<C, T>                            base_type;
		typedef basic_write_value<C, T>                           class_type;
		typedef typename traits_type::size_type                   size_type;
		typedef typename traits_type::string_type                 string_type;
		typedef typename traits_type::hkey_type                   hkey_type;
		typedef std::dynarray<uint8_t>                            blob_type;
		typedef typename traits_type::result_type                 result_type;

	public:
		basic_write_value();
		basic_write_value(class_type&& rhs);
		basic_write_value(hkey_type hkeyParent, string_type const& value_name);
		~basic_write_value() throw();

	public:
		bool                    del         ();
		bool                    set_uint32_t(uint32_t value);
		bool                    set_uint64_t(uint64_t value);
		bool                    set         (const char*    value);
		bool                    set         (const wchar_t* value);
		bool                    set         (const std::string&  value);
		bool                    set         (const std::wstring& value);
		bool                    set         (const char*    value, size_type length);
		bool                    set         (const wchar_t* value, size_type length);
		bool                    set         (void const* value, size_type cbValue);
		bool                    set         (uint32_t type, void const* value, size_type cbValue);

		template <typename Source>
		bool set(Source value, typename std::enable_if<
				std::is_unsigned<Source>::value && (sizeof(Source) == sizeof(uint32_t))
			, int>::type* = 0)
		{
			return set(value);
		}

		template <typename Source>
		bool set(Source value, typename std::enable_if<
				std::is_unsigned<Source>::value && (sizeof(Source) == sizeof(uint64_t))
			, int>::type* = 0)
		{
			return set(value);
		}

		template <typename Source>
		bool set(Source value, typename std::enable_if<
				is_stringable<char_type, typename boost::decay<Source>::type>::value
			, int>::type* = 0)
		{			
			return set(reg_dispatch<string_type>(value));
		}

	public:
		template <typename Source> class_type& operator =(Source s) { set(s); return *this; }
	};

	template <typename C, typename T>
	inline basic_write_value<C, T>::basic_write_value()
		: base_type()
	{ }

	template <typename C, typename T>
	inline basic_write_value<C, T>::basic_write_value(class_type&& rhs)
		: base_type(std::move(rhs))
	{ }

	template <typename C, typename T>
	inline basic_write_value<C, T>::basic_write_value(hkey_type hkeyParent, string_type const& value_name)
		: base_type(hkeyParent, value_name)
	{ } 

	template <typename C, typename T>
	inline basic_write_value<C, T>::~basic_write_value() throw()
	{ }

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::set_uint32_t(uint32_t value)
	{
		return set(REG_DWORD, &value, sizeof(value));
	}

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::set_uint64_t(uint64_t value)
	{
#ifndef REG_QWORD
		const DWORD REG_QWORD = 11;
#endif
		return set(REG_QWORD, &value, sizeof(value));
	}

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::set(const char* value)
	{
		return set(value, std::string::traits_type::length(value));
	}

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::set(const wchar_t* value)
	{
		return set(value, std::wstring::traits_type::length(value));
	}

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::set(const std::string& value)
	{
		return set(value.c_str(), value.size());
	}

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::set(const std::wstring& value)
	{
		return set(value.c_str(), value.size());
	}

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::set(const char* value, size_type length)
	{
		return set(REG_SZ, value, length * sizeof(char));
	}

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::set(const wchar_t* value, size_type length)
	{
		return set(REG_SZ, value, length * sizeof(wchar_t));
	}

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::set(void const* value, size_type cbValue)
	{
		return set(REG_BINARY, value, cbValue);
	}

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::set(uint32_t type, void const* value, size_type cbValue)
	{
		result_type res = traits_type::set_value(m_hkey, m_name.c_str(), type, value, cbValue);
		check_and_throw_exception("could not create value", res);
		return ERROR_SUCCESS == res;
	}

	template <typename C, typename T>
	inline bool basic_write_value<C, T>::del()
	{
		result_type res = traits_type::delete_value(m_hkey, m_name.c_str());

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

	typedef basic_read_value <char, reg_traits<char>>       read_value_a;
	typedef basic_read_value <wchar_t, reg_traits<wchar_t>> read_value_w;
	typedef basic_write_value<char, reg_traits<char>>       write_value_a;
	typedef basic_write_value<wchar_t, reg_traits<wchar_t>> write_value_w;
	typedef write_value_a                                   value_a;
	typedef write_value_w                                   value_w;
}}
