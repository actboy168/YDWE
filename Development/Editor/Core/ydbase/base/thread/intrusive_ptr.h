#pragma once

#include <ostream>
#include <cassert>

namespace base {

	template <class T>
	class intrusive_ptr
	{
	private:
		typedef intrusive_ptr this_type;

	public:
		typedef T element_type;

		intrusive_ptr() : px(0)
		{ }

		intrusive_ptr(T * p, bool add_ref = true)
			: px(p)
		{
			if (px != 0 && add_ref) intrusive_ptr_add_ref(px);
		}

		intrusive_ptr(intrusive_ptr const & rhs)
			: px(rhs.px)
		{
			if (px != 0) intrusive_ptr_add_ref(px);
		}

		~intrusive_ptr()
		{
			if (px != 0) intrusive_ptr_release(px);
		}

		intrusive_ptr(intrusive_ptr && rhs) : px(rhs.px)
		{
			rhs.px = 0;
		}

		intrusive_ptr & operator=(intrusive_ptr && rhs)
		{
			this_type(static_cast< intrusive_ptr && >(rhs)).swap(*this);
			return *this;
		}

		intrusive_ptr & operator=(intrusive_ptr const & rhs)
		{
			this_type(rhs).swap(*this);
			return *this;
		}

		intrusive_ptr & operator=(T * rhs)
		{
			this_type(rhs).swap(*this);
			return *this;
		}

		void reset()
		{
			this_type().swap(*this);
		}

		void reset(T * rhs)
		{
			this_type(rhs).swap(*this);
		}

		T * get() const
		{
			return px;
		}

		T & operator*() const
		{
			assert(px != 0);
			return *px;
		}

		T * operator->() const
		{
			assert(px != 0);
			return px;
		}

		operator bool() const
		{
			return px != 0;
		}

		void swap(intrusive_ptr & rhs)
		{
			T * tmp = px;
			px = rhs.px;
			rhs.px = tmp;
		}

	private:
		T * px;
	};

	template <class T, class U>
	inline bool operator==(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
	{
		return a.get() == b.get();
	}

	template <class T, class U>
	inline bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b)
	{
		return a.get() != b.get();
	}

	template <class T, class U>
	inline bool operator==(intrusive_ptr<T> const & a, U * b)
	{
		return a.get() == b;
	}

	template <class T, class U> inline bool operator!=(intrusive_ptr<T> const & a, U * b)
	{
		return a.get() != b;
	}

	template <class T, class U>
	inline bool operator==(T * a, intrusive_ptr<U> const & b)
	{
		return a == b.get();
	}

	template <class T, class U>
	inline bool operator!=(T * a, intrusive_ptr<U> const & b)
	{
		return a != b.get();
	}

	template <class T>
	inline bool operator==(intrusive_ptr<T> const & p, std::nullptr_t)
	{
		return p.get() == 0;
	}

	template <class T>
	inline bool operator==(std::nullptr_t, intrusive_ptr<T> const & p)
	{
		return p.get() == 0;
	}

	template <class T>
	inline bool operator!=(intrusive_ptr<T> const & p, std::nullptr_t)
	{
		return p.get() != 0;
	}

	template <class T>
	inline bool operator!=(std::nullptr_t, intrusive_ptr<T> const & p)
	{
		return p.get() != 0;
	}

	template <class T>
	inline bool operator<(intrusive_ptr<T> const & a, intrusive_ptr<T> const & b)
	{
		return std::less<T *>()(a.get(), b.get());
	}

	template <class T>
	void swap(intrusive_ptr<T> & lhs, intrusive_ptr<T> & rhs)
	{
		lhs.swap(rhs);
	}

	template <class T>
	T * get_pointer(intrusive_ptr<T> const & p)
	{
		return p.get();
	}

	template <class T, class U>
	intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const & p)
	{
		return static_cast<T *>(p.get());
	}

	template <class T, class U>
	intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const & p)
	{
		return const_cast<T *>(p.get());
	}

	template <class T, class U>
	intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const & p)
	{
		return dynamic_cast<T *>(p.get());
	}

	template <class E, class T, class Y>
	std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, intrusive_ptr<Y> const & p)
	{
		os << p.get();
		return os;
	}
}
