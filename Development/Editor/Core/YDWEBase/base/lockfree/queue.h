#pragma once

#include <cassert>
#include <memory>
#include <base/util/noncopyable.h>

#if defined(_MSC_VER)
#	include <intrin.h>
#	if defined(_M_IX86)
#		pragma intrinsic( _InterlockedExchange )
#		pragma intrinsic( _InterlockedCompareExchange )
#		undef _InterlockedExchangePointer
#		undef _InterlockedCompareExchangePointer
#		define _InterlockedExchangePointer(_Target, _Value) reinterpret_cast<void *>(static_cast<__w64 long>(_InterlockedExchange( \
	static_cast<long volatile *>(reinterpret_cast<__w64 long volatile *>(static_cast<void * volatile *>(_Target))), \
	static_cast<long>(reinterpret_cast<__w64 long>(static_cast<void *>(_Value))))))
#		define _InterlockedCompareExchangePointer(_Target, _Exchange, _Comparand) reinterpret_cast<void *>(static_cast<__w64 long>(_InterlockedCompareExchange( \
	static_cast<long volatile *>(reinterpret_cast<__w64 long volatile *>(static_cast<void * volatile *>(_Target))), \
	static_cast<long>(reinterpret_cast<__w64 long>(static_cast<void *>(_Exchange))), \
	static_cast<long>(reinterpret_cast<__w64 long>(static_cast<void *>(_Comparand))))))
#	elif defined(_M_X64)
#		pragma intrinsic( _InterlockedExchangePointer )
#		pragma intrinsic( _InterlockedCompareExchangePointer )
#	else
#	   error "Microsoft Visual C++ compiler: unsupported processor architecture"
#	endif
#else
#   error "Unsupported compiler"
#endif

namespace base { namespace lockfree { namespace details {

	template <typename T>
	class atomic_ptr_t 
		: private util::noncopyable
	{
	public:
		inline atomic_ptr_t () : ptr(nullptr) { }
		inline atomic_ptr_t (T *ptr_) : ptr(ptr_) { }
		inline ~atomic_ptr_t () { }
		inline void set  (T *ptr_) { this->ptr = ptr_; }
		inline T *xchg (T *val) { return (T*) _InterlockedExchangePointer ((PVOID*) &ptr, val); }
		inline T *cas  (T *cmp, T *val) { return (T*) _InterlockedCompareExchangePointer ( (volatile PVOID*) &ptr, val, cmp); }
	private:
		volatile T *ptr;
	};

	template <typename T, unsigned int _Size>
	struct yqueue_chunk_t
	{
		T values [_Size];
		yqueue_chunk_t<T, _Size>* next;
	};

	template <typename T, unsigned int _Size, class _Alloc>
	class yqueue_alloc 
		: public _Alloc::template rebind<yqueue_chunk_t<T, _Size> >::other
	{
	public:
		typedef yqueue_chunk_t<T, _Size>                      _Chunt;
		typedef typename _Alloc::template rebind<_Chunt>::other _Base;

	public:
		inline _Chunt*  New()
		{
			return _Base::allocate(1);
		}

		inline void Delete(_Chunt* p)
		{
			_Base::deallocate(p, 1);
		}
	};

	//  yqueue is an efficient queue implementation. The main goal is
	//  to minimise number of allocations/deallocations needed. Thus yqueue
	//  allocates/deallocates elements in batches of N.
	//
	//  yqueue allows one thread to use push/back function and another one 
	//  to use pop/front functions. However, user must ensure that there's no
	//  pop on the empty queue and that both threads don't access the same
	//  element in unsynchronised manner.
	//
	//  T is the type of the object in the queue.
	//  N is granularity of the queue (how many pushes have to be done till
	//  actual memory allocation is required).

	template <typename T, unsigned int _Size = 256, class _Alloc = std::allocator<T>>
	class yqueue
		: protected yqueue_alloc<T, _Size, _Alloc>
	{
	public:
		typedef yqueue_alloc<T, _Size, _Alloc> alloc;
		typedef typename T                     value_type;
		typedef typename T*                    pointer;		
		typedef typename value_type&           reference;
		typedef typename value_type const&     const_reference;

	protected:
		using alloc::New;
		using alloc::Delete;

	public:

		//  Create the queue.
		inline yqueue ()
		{
			begin_chunk = New ();
			assert (begin_chunk);
			begin_pos = 0;
			back_chunk = begin_chunk;
			back_pos = 0;
			end_chunk = begin_chunk;
			end_pos = 0;
		}

		//  Destroy the queue.
		inline ~yqueue ()
		{
			for (;;) {
				if (begin_chunk == end_chunk) {
					Delete(begin_chunk);
					break;
				} 
				chunk_t *o = begin_chunk;
				begin_chunk = begin_chunk->next;
				Delete(o);
			}

			chunk_t *sc = spare_chunk.xchg (nullptr);
			if (sc)
				Delete(sc);
		}

		//  Returns reference to the front element of the queue.
		//  If the queue is empty, behaviour is undefined.
		inline reference front ()
		{
			return begin_chunk->values [begin_pos];
		}

		inline const_reference front () const
		{
			return begin_chunk->values [begin_pos];
		}

		//  Returns reference to the back element of the queue.
		//  If the queue is empty, behaviour is undefined.
		inline reference back ()
		{
			return back_chunk->values [back_pos];
		}

		inline const_reference back () const
		{
			return back_chunk->values [back_pos];
		}

		//  Adds an element to the back end of the queue.
		inline void push ()
		{
			back_chunk = end_chunk;
			back_pos = end_pos;

			if (++end_pos != _Size)
				return;

			chunk_t *sc = spare_chunk.xchg (nullptr);
			if (sc) {
				end_chunk->next = sc;
			} else {
				end_chunk->next = New();
				assert (end_chunk->next);
			}
			end_chunk = end_chunk->next;
			end_pos = 0;
		}

		//  Removes an element from the front end of the queue.
		inline void pop ()
		{
			if (++ begin_pos == _Size) {
				chunk_t *o = begin_chunk;
				begin_chunk = begin_chunk->next;
				begin_pos = 0;

				//  'o' has been more recently used than spare_chunk,
				//  so for cache reasons we'll get rid of the spare and
				//  use 'o' as the spare.
				chunk_t *cs = spare_chunk.xchg (o);
				if (cs)
					Delete(cs);
			}
		}

		// 在pop线程调用，线程安全。
		inline bool empty () const
		{
			volatile chunk_t* _back_chunk   = back_chunk;
			volatile unsigned int _back_pos = back_pos;

			if ((begin_chunk == _back_chunk) && (begin_pos == _back_pos))
				return true;

			return false;
		}

	private:
		//  Individual memory chunk to hold N elements.
		typedef typename alloc::_Chunt chunk_t;

		//  Back position may point to invalid memory if the queue is empty,
		//  while begin & end positions are always valid. Begin position is
		//  accessed exclusively be queue reader (front/pop), while back and
		//  end positions are accessed exclusively by queue writer (back/push).
		chunk_t *    begin_chunk;
		unsigned int begin_pos;
		chunk_t *    back_chunk;
		unsigned int back_pos;
		chunk_t *    end_chunk;
		unsigned int end_pos;

		//  People are likely to produce and consume at similar rates.  In
		//  this scenario holding onto the most recently freed chunk saves
		//  us from having to call malloc/free.
		atomic_ptr_t<chunk_t> spare_chunk;

	private:
		yqueue(const yqueue&);
		yqueue& operator=(const yqueue&);
	};

}

template <typename T, unsigned int _Size = 256, class _Alloc = std::allocator<T>>
class queue 
	: private util::noncopyable
{
public:
	typedef queue<T, _Size, _Alloc>              _Myt;
	typedef details::yqueue<T, _Size, _Alloc>    _Container;
	typedef _Container                           container_type;
	typedef typename _Container::value_type      value_type;
	typedef typename _Container::reference       reference;
	typedef typename _Container::const_reference const_reference;

	queue() 
		: c() 
	{
		c.push(); 
		assert(empty());
	}

	~queue()
	{
		for (; !empty(); pop())
			;
		assert(empty());
	}

	bool empty() const
	{
		return (c.empty());
	}

	reference front()
	{
		return (c.front());
	}

	const_reference front() const
	{
		return (c.front());
	}

	reference back()
	{
		return (c.back());
	}

	const_reference back() const
	{
		return (c.back());
	}

	void push(value_type&& _Val)
	{
		new(&c.back()) T(std::move(_Val));
		c.push();
	}

	void push(const_reference _Val)
	{
		new(&c.back()) T(_Val);
		c.push();
	}

	void pop()
	{
		assert(!empty());
		c.front().~T();
		c.pop();
	}

	bool try_pop(reference _Val)
	{
		if (empty()) 
			return false;
		_Val.~T();
		new(&_Val) T(c.front());
		pop();
		return true;
	}

protected:
	_Container c;
};
}}

