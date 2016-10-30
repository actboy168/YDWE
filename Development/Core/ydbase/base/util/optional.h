#pragma once

#include <type_traits>

#if defined(_MSC_VER) && _MSC_VER < 1800
#include <boost/preprocessor/repetition.hpp>
#include <boost/preprocessor/punctuation/comma_if.hpp>
#include <boost/preprocessor/stringize.hpp>
#endif

namespace std {

	template<typename T>
	class optional
	{
		typedef  typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type data_t;
		
	public:
		optional()
			: initialized_(false)
		{ }

		optional(const T& v)
		{
			Create(v);
		}

		optional(T&& v)
			: initialized_(false)
		{
			Create(std::move(v));
		}

		~optional()
		{
			Destroy();
		}

		optional(const optional& other)
			: initialized_(false)
		{
			if (other.initialized_)
				Assign(other);
		}

		optional(optional&& other) 
			: initialized_(false)
		{
			if (other.initialized_)
			{
				Assign(std::move(other));
				other.Destroy();
			}
		}

		optional& operator=(optional &&other)
		{
			Assign(std::move(other));
			return *this;
		}

		optional& operator=(const optional &other)
		{
			Assign(other);
			return *this;
		}

#if defined(_MSC_VER) && _MSC_VER < 1800    
		_OPERATOR_BOOL() const {
			return (initialized_ ? _CONVERTIBLE_TO_TRUE : 0);
		}
#else	
		explicit operator bool() const {
			return initialized_;
		}
#endif

		T& get()
		{
			return operator*();
		}

		T const& get() const
		{
			return operator*();
		}

		T& operator*()
		{
			return *((T*)(&data_));
		}

		T const& operator*() const
		{
			if (initialized_)
			{
				return *((T*)(&data_));
			}

			throw std::exception("");
		}

		T* operator->()
		{
			return ((T*)(&data_));
		}

		T const* operator->() const
		{
			if (initialized_)
			{
				return ((T*)(&data_));
			}
			return 0;
		}

		bool operator == (const optional<T>& rhs) const
		{
			return (!bool(*this)) != (!rhs) ? false : (!bool(*this) ? true : (*(*this)) == (*rhs));
		}

		bool operator < (const optional<T>& rhs) const
		{
			return !rhs ? false : (!bool(*this) ? true : (*(*this) < (*rhs)));
		}

		bool operator != (const optional<T>& rhs)
		{
			return !(*this == (rhs));
		}

	private:
#if defined(_MSC_VER) && _MSC_VER < 1800
		void Create()
		{
			new (&data_) T();
			initialized_ = true;
		} 
		template <typename T1>
		void Create(T1 t1)
		{
			new (&data_) T(t1);
			initialized_ = true;
		}
#else
		template<class ... Args>
		void Create(Args&& ... args)
		{
			new (&data_) T(std::forward<Args>(args)...);
			initialized_ = true;
		}
#endif
		void Destroy()
		{
			if (initialized_)
			{
				initialized_ = false;
				((T*)(&data_))->~T();
			}
		}

		void Assign(const optional& other)
		{
			if (other.initialized_)
			{
				Copy(other.data_);
				initialized_ = true;
			}
			else
			{
				Destroy();
			}
		}

		void Assign(optional&& other)
		{
			if (other.initialized_)
			{
				Move(std::move(other.data_));
				initialized_ = true;
				other.Destroy();
			}
			else
			{
				Destroy();
			}
		}

		void Move(data_t&& val)
		{
			Destroy();
			new (&data_) T(std::move(*((T*)

				(&val))));
		}

		void Copy(const data_t& val)
		{
			Destroy();
			new (&data_) T(*((T*)(&val)));
		}

	private:
		bool initialized_;
		data_t data_;
	};
}
