#pragma once

#include <base/win/registry/key.h>

namespace base { namespace registry {

	class predefined_key
	{
	public:
		typedef HKEY   hkey_type;

	public:
		predefined_key(hkey_type hkey)
			: m_hkey(hkey)
		{ }

		hkey_type handle() const
		{
			return m_hkey;
		}

	private:
		hkey_type m_hkey;
	};

	class read_predefined_key
		: public predefined_key
	{
	public:
		read_predefined_key(hkey_type hkey)
			: predefined_key(hkey)
		{ }
	};

	class write_predefined_key
		: public predefined_key
	{
	public:
		write_predefined_key(hkey_type hkey)
			: predefined_key(hkey)
		{ }
	};

	template <typename C>
	inline basic_read_key<C> operator/(const read_predefined_key& lhs, const std::basic_string<C>& rhs)
	{
		return basic_read_key<C>(lhs.handle(), rhs);
	}

	template <typename C>
	inline basic_read_key<C> operator/(const read_predefined_key& lhs, const C* rhs)
	{
		return basic_read_key<C>(lhs.handle(), rhs);
	}

	template <typename C>
	inline basic_write_key<C> operator/(const write_predefined_key& lhs, const std::basic_string<C>& rhs)
	{
		return basic_write_key<C>(lhs.handle(), rhs);
	}

	template <typename C>
	inline basic_write_key<C> operator/(const write_predefined_key& lhs, const C* rhs)
	{
		return basic_write_key<C>(lhs.handle(), rhs);
	}

	inline read_predefined_key read_current_user()
	{
		return read_predefined_key(HKEY_CURRENT_USER);
	}

	inline read_predefined_key read_local_machine()
	{
		return read_predefined_key(HKEY_LOCAL_MACHINE);
	}
	
	inline write_predefined_key write_current_user()
	{
		return write_predefined_key(HKEY_CURRENT_USER);
	}

	inline write_predefined_key write_local_machine()
	{
		return write_predefined_key(HKEY_LOCAL_MACHINE);
	}
}}
