#pragma once

#include <base/hook/assembler/register.h>
#include <array>
#include <cassert>

namespace base { 
namespace hook { namespace assembler {

	class operand
	{
	public:
		explicit operand(reg r) 
		{
			set_modrm(3, r);
		}

		explicit operand(reg base, int8_t disp)
		{
			if (disp == 0 && !base.is(ebp))
			{
				set_modrm(0, base);
				if (base.is(esp)) 
				{
					set_sib(esp, base);
				}
			} 
			else
			{
				set_modrm(1, base);
				if (base.is(esp)) 
				{ 
					set_sib(esp, base); 
				}
				set_disp8(disp);
			}
		}

		void set_modrm(int mod, reg rm) 
		{
			assert((mod & -4) == 0);
			buf_[0] = (mod & 0x03) << 6 | rm.code();
			len_ = 1;
		}

		void set_disp8(int8_t disp) 
		{
			assert(len_ == 1 || len_ == 2);
			*reinterpret_cast<int8_t*>(&buf_[len_++]) = disp;
		}

		void set_sib(reg index, reg base) 
		{
			assert(len_ == 1);
			assert(!index.is(esp) || base.is(esp));
			buf_[1] = 0 << 6 | index.code() << 3 | base.code();
			len_ = 2;
		}

		uint8_t buf_[6];
		size_t  len_;
	};
}}
}
