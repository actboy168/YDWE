#pragma once

#include <unordered_map>
#include <slk/utility/object_id.h>

namespace slk
{
	namespace ignore_case {

		template <class T>
		struct less : public std::binary_function<T, T, bool>
		{
			bool operator()(const T& lft, const T& rht) const
			{
				return (lft < _Right);
			}
		};

		template <>
		struct less<char> : public std::binary_function<char, char, bool>
		{
			bool operator()(const char& lft, const char& rht) const
			{
				return (tolower(static_cast<unsigned char>(lft)) < tolower(static_cast<unsigned char>(rht)));
			}
		};

		template <>
		struct less<std::string> : public std::binary_function<std::string, std::string, bool>
		{
			bool operator()(const std::string& lft, const std::string& rht) const
			{
				return std::lexicographical_compare(lft.begin(), lft.end(), rht.begin(), rht.end(), less<char>());
			}
		};

		template <class T>
		struct equal_to : public std::binary_function<T, T, bool>
		{
			bool operator()(const T& lft, const T& rht) const
			{
				return (lft == rht);
			}
		};

		template <>
		struct equal_to<char> : public std::binary_function<char, char, bool>
		{
			bool operator()(const char& lft, const char& rht) const
			{
				return (tolower(static_cast<unsigned char>(lft)) == tolower(static_cast<unsigned char>(rht)));
			}
		};

		template <>
		struct equal_to<std::string> : public std::binary_function<std::string, std::string, bool>
		{
			bool operator()(const std::string& lft, const std::string& rht) const
			{
				auto _First1 = lft.begin();
				auto _Last1  = lft.end();
				auto _First2 = rht.begin();
				auto _Last2  = rht.end();

				for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, ++_First2)
					if (!equal_to<char>()(*_First1, *_First2))
						return (false);

				return (_First1 == _Last1 && _First2 == _Last2);
			}
		};

		template <class _Kty>
		class hash;

		template<>
		class hash<std::string> : public std::unary_function<std::string, size_t>
		{
		public:
			typedef std::string _Kty;

			size_t operator()(const _Kty& _Keyval) const
			{
				const unsigned char *_First = (const unsigned char*)_Keyval.c_str();
				size_t _Count = _Keyval.size();
				const size_t _FNV_offset_basis = 2166136261U;
				const size_t _FNV_prime = 16777619U;

				size_t _Val = _FNV_offset_basis;
				for (size_t _Next = 0; _Next < _Count; ++_Next)
				{
					_Val ^= (size_t)tolower(_First[_Next]);
					_Val *= _FNV_prime;
				}

				return (_Val);
			}
		};
	}

	namespace detail {
		template <class _Kty>
		class hash : public std::unary_function<_Kty, size_t>
		{
		public:
			size_t operator()(const _Kty& _Keyval) const
			{
				const unsigned char *_First = (const unsigned char*)&_Keyval;
				size_t _Count = sizeof(_Kty);
				const size_t _FNV_offset_basis = 2166136261U;
				const size_t _FNV_prime = 16777619U;

				size_t _Val = _FNV_offset_basis;
				for (size_t _Next = 0; _Next < _Count; ++_Next)
				{
					_Val ^= (size_t)_First[_Next];
					_Val *= _FNV_prime;
				}

				return (_Val);
			}
		};

		template<>
		class hash<std::string> : public std::unary_function<std::string, size_t>
		{
		public:
			typedef std::string _Kty;

			size_t operator()(const _Kty& _Keyval) const
			{
				const unsigned char *_First = (const unsigned char*)_Keyval.c_str();
				size_t _Count = _Keyval.size();
				const size_t _FNV_offset_basis = 2166136261U;
				const size_t _FNV_prime = 16777619U;

				size_t _Val = _FNV_offset_basis;
				for (size_t _Next = 0; _Next < _Count; ++_Next)
				{
					_Val ^= (size_t)_First[_Next];
					_Val *= _FNV_prime;
				}

				return (_Val);
			}
		};
	}

	template <class _Key, class _Value>
	struct HashTable
	{
		typedef std::unordered_map<_Key, _Value, detail::hash<_Key>>                                   Type;
		typedef std::unordered_map<_Key, _Value, ignore_case::hash<_Key>, ignore_case::equal_to<_Key>> IType;
	};
}
