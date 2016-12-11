#pragma once

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

	template <typename C>
	inline basic_key<C> operator/(const predefined_key& lhs, const std::basic_string<C>& rhs)
	{
		return basic_key<C>(lhs.handle(), rhs);
	}

	template <typename C>
	inline basic_key<C> operator/(const predefined_key& lhs, const C* rhs)
	{
		return basic_key<C>(lhs.handle(), basic_key<C>::string_type(), rhs, open_access::none);
	}

	inline predefined_key current_user()
	{
		return predefined_key(HKEY_CURRENT_USER);
	}

	inline predefined_key local_machine()
	{
		return predefined_key(HKEY_LOCAL_MACHINE);
	}
}}
