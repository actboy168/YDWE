#pragma once

#include <cassert>
#include <memory>

namespace base { namespace lockfree {

#if defined(_MSC_VER)
#	include <intrin.h>
#	if defined(_M_IX86)
#		pragma intrinsic( _InterlockedExchange )
#		undef _InterlockedExchangePointer
#		define _InterlockedExchangePointer(_Target, _Value) reinterpret_cast<void *>(static_cast<__w64 long>(_InterlockedExchange( \
	static_cast<long volatile *>(reinterpret_cast<__w64 long volatile *>(static_cast<void * volatile *>(_Target))), \
	static_cast<long>(reinterpret_cast<__w64 long>(static_cast<void *>(_Value))))))
#	elif defined(_M_X64)
#		pragma intrinsic( _InterlockedExchangePointer )
#	else
#		error "Microsoft Visual C++ compiler: unsupported processor architecture"
#	endif
	template <typename T>
	class atomic;
	
	template <typename T>
	class atomic<T*>
	{
	public:
		inline atomic () : ptr(nullptr) { }
		inline atomic (T *ptr_) : ptr(ptr_) { }
		inline ~atomic () { }
		inline T *exchange (T *val) { return (T*) _InterlockedExchangePointer ((PVOID*) &ptr, val); }
	private:
		volatile T *ptr;
	private:
		atomic(const atomic&);
		atomic& operator=(const atomic&);
	};
#else
#	include <atomic>
	using std::atomic;
#endif

	template <typename T, ::std::size_t N>
	struct queue_chunk
	{
		T values [N];
		queue_chunk<T, N>* next;
	};

	template <typename T, ::std::size_t N, typename Alloc>
	class queue_alloc
		: public Alloc::template rebind<queue_chunk<T, N> >::other
	{
	public:
		typedef queue_chunk<T, N>                                  chunt_type;
		typedef typename Alloc::template rebind<chunt_type>::other base_type;

	public:
		inline chunt_type*  allocate()
		{
			return base_type::allocate(1);
		}

		inline void deallocate(chunt_type* p)
		{
			base_type::deallocate(p, 1);
		}
	};

	template <typename T, ::std::size_t N = 256, typename Alloc = ::std::allocator<T>>
	class queue
		: public queue_alloc<T, N, Alloc>
	{
	public:
		typedef queue_alloc<T, N, Alloc>        alloc_type;
		typedef typename alloc_type::chunt_type chunt_type;
		typedef T                               value_type;
		typedef value_type*                     pointer;
		typedef value_type&                     reference;
		typedef value_type const&               const_reference;

	public:
		inline queue ()
		{
			begin_chunk = alloc_type::allocate();
			assert (begin_chunk);
			begin_pos = 0;
			back_chunk = begin_chunk;
			back_pos = 0;
			end_chunk = begin_chunk;
			end_pos = 0;
			do_push();
			assert(empty());
		}

		inline ~queue ()
		{
			for (;;) {
				if (begin_chunk == end_chunk) {
					alloc_type::deallocate(begin_chunk);
					break;
				}
				chunt_type *o = begin_chunk;
				begin_chunk = begin_chunk->next;
				alloc_type::deallocate(o);
			}

			chunt_type *sc = spare_chunk.exchange(nullptr);
			if (sc)
				alloc_type::deallocate(sc);
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

		void push(value_type&& val)
		{
			new(&back()) T(::std::move(val));
			do_push();
		}

		void push(const_reference val)
		{
			new(&back()) T(val);
			do_push();
		}

		void pop()
		{
			assert(!empty());
			front().~T();
			do_pop();
		}

		bool try_pop(reference val)
		{
			if (empty())
				return false;
			val.~T();
			new(&val) T(front());
			pop();
			return true;
		}

		inline void do_push ()
		{
			back_chunk = end_chunk;
			back_pos = end_pos;

			if (++end_pos != N)
				return;

			chunt_type *sc = spare_chunk.exchange(nullptr);
			if (sc) {
				end_chunk->next = sc;
			} else {
				end_chunk->next = alloc_type::allocate();
				assert (end_chunk->next);
			}
			end_chunk = end_chunk->next;
			end_pos = 0;
		}

		inline void do_pop ()
		{
			if (++ begin_pos == N) {
				chunt_type *o = begin_chunk;
				begin_chunk = begin_chunk->next;
				begin_pos = 0;

				chunt_type *cs = spare_chunk.exchange(o);
				if (cs)
					alloc_type::deallocate(cs);
			}
		}

		// It's safe in pop thread.
		inline bool empty () const
		{
			volatile chunt_type* _back_chunk = back_chunk;
			volatile size_t      _back_pos   = back_pos;

			if ((begin_chunk == _back_chunk) && (begin_pos == _back_pos))
				return true;

			return false;
		}

	private:
		chunt_type* begin_chunk;
		size_t      begin_pos;
		chunt_type* back_chunk;
		size_t      back_pos;
		chunt_type* end_chunk;
		size_t      end_pos;

		atomic<chunt_type*> spare_chunk;

	private:
		queue(const queue&);
		queue& operator=(const queue&);
	};
}}
