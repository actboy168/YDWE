#pragma once

#include <cassert>
#include <memory>
#include <base/util/noncopyable.h>

namespace base { namespace lockfree { namespace detail {

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
#		error "Microsoft Visual C++ compiler: unsupported processor architecture"
#	endif
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
		inline T *cas  (T *cmp, T *val) { return (T*) _InterlockedCompareExchangePointer ((volatile PVOID*) &ptr, val, cmp); }
	private:
		volatile T *ptr;
	};
#else
#include <atomic>
	template <typename T>
	class atomic_ptr_t 
		: private std::atomic<T*>
	{
		typedef std::atomic<T*> base_type;
	public:
		inline atomic_ptr_t () : base_type() { }
		inline atomic_ptr_t (T *ptr_) : base_type(ptr_) { }
		inline ~atomic_ptr_t () { }
		inline void set  (T *ptr_) { base_type::store(ptr_); }
		inline T *xchg (T *val) { return base_type::exchange(val); }
		inline T *cas  (T *cmp, T *val) { bool suc = base_type::compare_exchange_strong(cmp, val); return cmp; }
	};
#endif

	template <typename T, ::std::size_t N>
	struct queue_chunk_t
	{
		T values [N];
		queue_chunk_t<T, N>* next;
	};

	template <typename T, ::std::size_t N, typename Alloc>
	class queue_alloc 
		: public Alloc::template rebind<queue_chunk_t<T, N> >::other
	{
	public:
		typedef queue_chunk_t<T, N>                                chunt_type;
		typedef typename Alloc::template rebind<chunt_type>::other base_type;

	public:
		inline chunt_type*  New()
		{
			return base_type::allocate(1);
		}

		inline void Delete(chunt_type* p)
		{
			base_type::deallocate(p, 1);
		}
	};

	template <typename T, ::std::size_t N = 256, typename Alloc = ::std::allocator<T>>
	class queue
		: protected queue_alloc<T, N, Alloc>
	{
	public:
		typedef queue_alloc<T, N, Alloc>       alloc_type;
		typedef T                              value_type;
		typedef value_type*                    pointer;
		typedef value_type&                    reference;
		typedef value_type const&              const_reference;

	protected:
		using alloc_type::New;
		using alloc_type::Delete;

	public:
		inline queue ()
		{
			begin_chunk = New();
			assert (begin_chunk);
			begin_pos = 0;
			back_chunk = begin_chunk;
			back_pos = 0;
			end_chunk = begin_chunk;
			end_pos = 0;
		}

		inline ~queue ()
		{
			for (;;) {
				if (begin_chunk == end_chunk) {
					Delete(begin_chunk);
					break;
				} 
				chunt_type *o = begin_chunk;
				begin_chunk = begin_chunk->next;
				Delete(o);
			}

			chunt_type *sc = spare_chunk.xchg(nullptr);
			if (sc)
				Delete(sc);
		}

		inline reference front ()
		{
			return begin_chunk->values [begin_pos];
		}

		inline const_reference front () const
		{
			return begin_chunk->values [begin_pos];
		}

		inline reference back ()
		{
			return back_chunk->values [back_pos];
		}

		inline const_reference back () const
		{
			return back_chunk->values [back_pos];
		}

		inline void push ()
		{
			back_chunk = end_chunk;
			back_pos = end_pos;

			if (++end_pos != N)
				return;

			chunt_type *sc = spare_chunk.xchg(nullptr);
			if (sc) {
				end_chunk->next = sc;
			} else {
				end_chunk->next = New();
				assert (end_chunk->next);
			}
			end_chunk = end_chunk->next;
			end_pos = 0;
		}

		inline void pop ()
		{
			if (++ begin_pos == N) {
				chunt_type *o = begin_chunk;
				begin_chunk = begin_chunk->next;
				begin_pos = 0;

				chunt_type *cs = spare_chunk.xchg(o);
				if (cs)
					Delete(cs);
			}
		}

		// 在pop线程调用，线程安全。
		inline bool empty () const
		{
			volatile chunt_type* _back_chunk = back_chunk;
			volatile unsigned int _back_pos  = back_pos;

			if ((begin_chunk == _back_chunk) && (begin_pos == _back_pos))
				return true;

			return false;
		}

	private:
		typedef typename alloc_type::chunt_type chunt_type;

		chunt_type*  begin_chunk;
		unsigned int begin_pos;
		chunt_type*  back_chunk;
		unsigned int back_pos;
		chunt_type*  end_chunk;
		unsigned int end_pos;

		atomic_ptr_t<chunt_type> spare_chunk;

	private:
		queue(const queue&);
		queue& operator=(const queue&);
	};

}

template <typename T, unsigned int N = 256, class Alloc = std::allocator<T>>
class queue 
	: private util::noncopyable
{
public:
	typedef queue<T, N, Alloc>                       class_type;
	typedef detail::queue<T, N, Alloc>               container_type;
	typedef typename container_type::value_type      value_type;
	typedef typename container_type::reference       reference;
	typedef typename container_type::const_reference const_reference;

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
	container_type c;
};
}}
