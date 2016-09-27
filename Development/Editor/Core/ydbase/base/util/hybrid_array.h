#pragma once

#include <base/config.h>
#include <algorithm>
#include <limits>
#include <iterator>
#include <stdexcept>
#include <type_traits>

namespace base {

	template <typename T, size_t SIZE>
	class hybrid_array
	{
	public:
		typedef       T                               value_type;
		typedef       T&                              reference;
		typedef const T&                              const_reference;
		typedef       T*                              pointer;
		typedef const T*                              const_pointer;
		typedef       T*                              iterator;
		typedef const T*                              const_iterator;
		typedef std::reverse_iterator<iterator>       reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
		typedef size_t                                size_type;
		typedef ptrdiff_t                             difference_type;

		static_assert(std::is_pod<value_type>::value, "hybrid_array::value_type must be a pod type.");

		hybrid_array() 
			: size_(0)
			, capacity_(SIZE)
			, ptr_(data_) 
		{ }

		~hybrid_array() 
		{
			if (ptr_ != data_) 
			{
				delete [] ptr_;
			}
		}

		iterator               begin()                       { return ptr_; }
		const_iterator         begin()                 const { return ptr_; }
		const_iterator         cbegin()                const { return ptr_; }
		iterator               end()                         { return ptr_ + size_; }
		const_iterator         end()                   const { return ptr_ + size_; }
		const_iterator         cend()                  const { return ptr_ + size_; }
		reverse_iterator       rbegin()                      { return reverse_iterator(end()); }
		const_reverse_iterator rbegin()                const { return reverse_iterator(end()); }
		reverse_iterator       rend()                        { return reverse_iterator(begin()); }
		const_reverse_iterator rend()                  const { return reverse_iterator(begin()); }
		size_type              size()                  const { return size_; }
		size_type              max_size()              const { return capacity_; }
		bool                   empty()                 const { return size_ == 0; }
		reference              operator[](size_type n)       { return ptr_[n]; }
		const_reference        operator[](size_type n) const { return ptr_[n]; }
		reference              front()                       { return ptr_[0]; }
		const_reference        front()                 const { return ptr_[0]; }
		reference              back()                        { return ptr_[size_-1]; }
		const_reference        back()                  const { return ptr_[size_-1]; }
		const_reference        at(size_type n)         const { check(n); return ptr_[n]; }
		reference              at(size_type n)               { check(n); return ptr_[n]; }
		T*                     data()                        { return ptr_; }
		const T*               data()                  const { return ptr_; }
		void                   fill(const T& v)              { fill_n(begin(), size(), v); }
		size_type              capacity() const              { return capacity_; }
		void                   clear()                       { size_ = 0; }

		void resize(size_type new_size) 
		{
			if (new_size > capacity_)
			{
				grow(new_size);
			}
			size_ = new_size;
		}

		void reserve(size_type capacity)
		{
			if (capacity > capacity_)
			{
				grow(capacity);
			}
		}

		void push_back(const_reference value)
		{
			if (size_ == capacity_)
			{
				grow(size_ + 1);
			}
			ptr_[size_++] = value;
		}

		void append(const_iterator first, const_iterator last)
		{
			difference_type num_elements = last - first;
			if (size_ + num_elements > capacity_)
			{
				grow(size_ + num_elements);
			}
			std::uninitialized_copy(first, last, ptr_ + size_);
			size_ += num_elements;
		}

	private:
		void grow(size_type size) 
		{
			capacity_ = (std::max)(size, capacity_ + capacity_ / 2);
			pointer p = new value_type[capacity_];
			std::uninitialized_copy(ptr_, ptr_ + size_, p);
			if (ptr_ != data_)
			{
				delete [] ptr_;
			}
			ptr_ = p;
		}

		void check(size_type n)
		{ 
			if (n >= size_)
			{
				throw std::out_of_range("hybrid_array"); 
			}
		}

		hybrid_array(const hybrid_array&);
		hybrid_array& operator=(const hybrid_array&);

	private:
		size_type  size_;
		size_type  capacity_;
		pointer    ptr_;
		value_type data_[SIZE];
	};

}
