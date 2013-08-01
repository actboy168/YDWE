#pragma once

#include <ydwe/hook/assembler/register.h>
#include <array>
#include <cassert>

_BASE_BEGIN namespace hook { namespace assembler {
	template <size_t BufSizeT = 256>
	class writer : public std::array<uint8_t, BufSizeT>
	{
		typedef std::array<uint8_t, BufSizeT> _Mybase;
	public:
		writer()
			: cur_(_Mybase::data())
		{ }

		template <class T>
		inline void emit(T val) 
		{
			static_assert(std::is_integral<T>::value, "emit's param must be integer.");
			assert((size() + sizeof T) <= BufSizeT);

			*reinterpret_cast<T*>(cur_) = val;
			cur_ += sizeof T;
		}

		void mov(reg dst, uint32_t imm32) 
		{
			emit((uint8_t)(0xB8 | dst.code()));
			emit(imm32);
		}

		void push(uint32_t imm32)
		{
			emit((uint8_t)0x68);
			emit(imm32);
		}

		void push(reg src) 
		{
			emit(0x50 | src.code());
		}

		void jmp(uintptr_t jmp_dst, uintptr_t jmp_src) 
		{
			emit((uint8_t)0xE9);
			emit((uint32_t)(jmp_dst - (jmp_src + 5)));
		}

		void call(uintptr_t call_dst, uintptr_t call_src) 
		{
			emit((uint8_t)0xE8);
			emit((uint32_t)(call_dst) - (call_src + 5));
		}

		size_t size() const
		{
			return cur_ - _Mybase::data();
		}

	private:
		uint8_t* cur_;
	};
}}}
