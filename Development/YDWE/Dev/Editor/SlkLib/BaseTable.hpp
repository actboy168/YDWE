#pragma once

#include <map>
#include <unordered_map>
#include "ObjectId.hpp"
#include "Buffer.hpp"

namespace slk
{
	namespace detail
	{
		struct IStringLess
		{
			struct CharLess : public std::binary_function<const char, const char, bool>
			{
				bool operator()(const char c1,const char c2) const
				{
					return tolower(static_cast<unsigned char>(c1)) 
						< tolower(static_cast<unsigned char>(c2));
				}    
			};

			bool operator () (const std::string& s1, const std::string& s2) const
			{
				return std::lexicographical_compare(
					s1.begin(),s1.end(),
					s2.begin(),s2.end(),
					CharLess());
			} 
		};

		struct IStringEqual
		{
			struct CharEqual : public std::binary_function<const char, const char, bool>
			{
				bool operator()(const char c1,const char c2) const
				{
					return tolower(static_cast<unsigned char>(c1)) 
						== tolower(static_cast<unsigned char>(c2));
				}    
			};

			bool operator () (const std::string& s1, const std::string& s2) const
			{
				auto _First1 = s1.begin();
				auto _Last1  = s1.end();
				auto _First2 = s2.begin();
				auto _Last2  = s2.end();

				for (; _First1 != _Last1 && _First2 != _Last2; ++_First1, ++_First2)
					if (!CharEqual()(*_First1, *_First2))
						return (false);
				return (_First1 == _Last1 && _First2 == _Last2);
			} 
		};

		inline size_t HashSeq(const unsigned char *_First, size_t _Count)
		{
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

		inline size_t HashSeqI(const unsigned char *_First, size_t _Count)
		{
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

		struct IStringHash: public std::unary_function<std::string, size_t>
		{
			size_t operator() (const std::string& _Keyval) const
			{
				return HashSeqI(
					(const unsigned char*)_Keyval.c_str(), 
					_Keyval.size()*sizeof(char));
			}
		};

		template <class _Ty>
		struct Hash: public std::unary_function<_Ty, size_t>
		{
			size_t operator() (const _Ty& _Keyval) const
			{
				return HashSeq(
					(const unsigned char*)&_Keyval, 
					sizeof(_Keyval));
			}
		};

		template <>
		struct Hash<std::string>: public std::unary_function<std::string, size_t>
		{
			size_t operator() (const std::string& _Keyval) const
			{	
				return HashSeq(
					(const unsigned char*)_Keyval.c_str(), 
					_Keyval.size()*sizeof(char));
			}
		};
	}

	enum HASH_TABLE_TYPE
	{
		MAP,
		UNORDERED_MAP,
	};

	template <class _Key, class _Value, HASH_TABLE_TYPE _Type = UNORDERED_MAP>
	struct HashTable;

	template <class _Key, class _Value>
	struct HashTable<_Key, _Value, MAP>
	{
		typedef std::map<_Key, _Value>                      Type;
		typedef std::map<_Key, _Value, detail::IStringLess> IType;
	};

	template <class _Key, class _Value>
	struct HashTable<_Key, _Value, UNORDERED_MAP>
	{
		typedef std::unordered_map<_Key, _Value, detail::Hash<_Key>>                        Type;
		typedef std::unordered_map<_Key, _Value, detail::IStringHash, detail::IStringEqual> IType;
	};

	template <class T, class _Value>
	class BaseTable: public HashTable<ObjectId, _Value>::Type
	{
	typedef typename HashTable<ObjectId, _Value>::Type _Base;
	public:
		_Value& operator[](const std::string& object_id)
		{
			return _Base::operator[](ObjectId(object_id));
		}
		_Value& operator[](const uint32_t& object_id)
		{
			return _Base::operator[](ObjectId(object_id));
		}
		_Value& operator[](const ObjectId& object_id)
		{
			return _Base::operator[](object_id);
		}

		template <class _Reader>
		void Read(buffer&& buf)
		{
			_Reader::Read(std::forward<buffer>(buf), *static_cast<T*>(this));
		}

		bool getValueById(ObjectId const& id, _Value const** ppval) const
		{
			if (!ppval)
			{
				return false;
			}

			auto const& It = find(id);
			if (It == end())
			{
				return false;
			}

			*ppval = &It->second;
			return true;
		}
	};
}
