#pragma once

#include <base/config.h>
#include <cstdint>
#include <cassert>

namespace base { 
namespace hook { namespace assembler {
	struct reg
	{
		static const int c_num_registers = 8;

		bool is_valid() const 
		{ 
			return 0 <= code_ && code_ < c_num_registers; 
		}

		bool is(reg r) const
		{ 
			return code_ == r.code_; 
		}

		bool is_byte_register() const 
		{ 
			return code_ <= 3;
		}

		uint8_t code() const 
		{
			assert(is_valid());
			return code_;
		}

		uint8_t code_;
	};

	const uint8_t c_register_eax_code = 0;
	const uint8_t c_register_ecx_code = 1;
	const uint8_t c_register_edx_code = 2;
	const uint8_t c_register_ebx_code = 3;
	const uint8_t c_register_esp_code = 4;
	const uint8_t c_register_ebp_code = 5;
	const uint8_t c_register_esi_code = 6;
	const uint8_t c_register_edi_code = 7;
	const uint8_t c_register_no_reg_code = 0xFF;

	const reg eax    = { c_register_eax_code };
	const reg ecx    = { c_register_ecx_code };
	const reg edx    = { c_register_edx_code };
	const reg ebx    = { c_register_ebx_code };
	const reg esp    = { c_register_esp_code };
	const reg ebp    = { c_register_ebp_code };
	const reg esi    = { c_register_esi_code };
	const reg edi    = { c_register_edi_code };
	const reg no_reg = { c_register_no_reg_code };
}}
}
