#pragma once

#include <array>
#include <cassert>
#include <cstdint>

namespace base { 
namespace warcraft3 { namespace jass {

	struct opcode 
	{
		union 
		{
			uint8_t r3;
			uint8_t nfarg;
		};
		union 
		{
			uint8_t r2;
			uint8_t user_return;
		};
		union 
		{
			uint8_t r1;
			uint8_t return_type;
		};
		uint8_t opcode_type;

		union 
		{
			uintptr_t      arg;
			struct opcode* dest;
		};
	};

	enum OPCODES 
	{ 
		OPTYPE_ENDPROGRAM   = 0x01,
		OPTYPE_UNKNOW_02    = 0x02,
		OPTYPE_FUNCTION     = 0x03, // _ _ rettype funcname
		OPTYPE_ENDFUNCTION  = 0x04,
		OPTYPE_LOCAL        = 0x05, // _ _ type name
		OPTYPE_GLOBAL       = 0x06,
		OPTYPE_CONSTANT     = 0x07,
		OPTYPE_POPFUNCARG   = 0x08, // _ srcargi type destvar
		OPTYPE_UNKNOW_09    = 0x09,
		OPTYPE_UNKNOW_0A    = 0x0A,
		OPTYPE_CLEANSTACK   = 0x0B, // _ _ nargs _
		OPTYPE_LITERAL      = 0x0C, // _ type destreg srcvalue
		OPTYPE_SETRET       = 0x0D, // _ srcreg _ _
		OPTYPE_GETVAR       = 0x0E, // _ type destreg srcvar
		OPTYPE_CODE         = 0x0F,
		OPTYPE_GETARRAY     = 0x10,
		OPTYPE_SETVAR       = 0x11,	// _ _ srcreg destvar
		OPTYPE_SETARRAY     = 0x12,
		OPTYPE_PUSH         = 0x13, // _ _ srcreg _
		OPTYPE_POP          = 0x14,
		OPTYPE_NATIVE       = 0x15, // _ _ _ fn
		OPTYPE_JASSCALL     = 0x16, // _ _ _ fn
		OPTYPE_I2R          = 0x17,
		OPTYPE_AND          = 0x18,
		OPTYPE_OR           = 0x19,
		OPTYPE_EQUAL        = 0x1A,
		OPTYPE_NOTEQUAL     = 0x1B, // check
		OPTYPE_LESSEREQUAL  = 0x1C,
		OPTYPE_GREATEREQUAL = 0x1D,
		OPTYPE_LESSER       = 0x1E,
		OPTYPE_GREATER      = 0x1F,
		OPTYPE_ADD          = 0x20,
		OPTYPE_SUB          = 0x21,          
		OPTYPE_MUL          = 0x22,          
		OPTYPE_DIV          = 0x23,          
		OPTYPE_MOD          = 0x24,
		OPTYPE_NEGATE       = 0x25,
		OPTYPE_NOT          = 0x26,
		OPTYPE_RETURN       = 0x27,	// _ _ _ _
		OPTYPE_JUMPTARGET   = 0x28,
		OPTYPE_JUMPIFTRUE   = 0x29,
		OPTYPE_JUMPIFFALSE  = 0x2A,
		OPTYPE_JUMP         = 0x2B,
	};
	
	enum OPCODE_VARIABLE_TYPE
	{
		OPCODE_VARIABLE_NOTHING = 0,   // "nothing"
		OPCODE_VARIABLE_UNKNOWN,       // "unknown"
		OPCODE_VARIABLE_NULL,          // "null"
		OPCODE_VARIABLE_CODE,          // "code"
		OPCODE_VARIABLE_INTEGER,       // "integer"
		OPCODE_VARIABLE_REAL,          // "real"
		OPCODE_VARIABLE_STRING,        // "string"
		OPCODE_VARIABLE_HANDLE,        // "handle"
		OPCODE_VARIABLE_BOOLEAN,       // "boolean"
		OPCODE_VARIABLE_INTEGER_ARRAY, // "integer array"
		OPCODE_VARIABLE_REAL_ARRAY,    // "real array"
		OPCODE_VARIABLE_STRING_ARRAY,  // "string array"
		OPCODE_VARIABLE_HANDLE_ARRAY,  // "handle array"
		OPCODE_VARIABLE_BOOLEAN_ARRAY, // "boolean array"
	};

	typedef uint32_t StringID;
	const uint8_t RETURN_REGISTER = 0x00;
	
	template <size_t BufSizeT = 256>
	class opcode_writer : public std::array<opcode, BufSizeT>
	{
		typedef std::array<opcode, BufSizeT> mybase;
	public:
		opcode_writer()
			: cur_(0)
		{ }

		size_t size() const
		{
			return cur_;
		}

		void emit(uint8_t opcode_type, uint8_t r1 = 0, uint8_t r2 = 0, uint8_t r3 = 0, uintptr_t arg = 0) 
		{
			assert(cur_ < BufSizeT);

			mybase::operator [](cur_).opcode_type = opcode_type;
			mybase::operator [](cur_).r1          = r1;
			mybase::operator [](cur_).r2          = r2;
			mybase::operator [](cur_).r3          = r3;
			mybase::operator [](cur_).arg         = arg;
			cur_++;
		}

		void func(StringID name, OPCODE_VARIABLE_TYPE ret_type)
		{
			emit(OPTYPE_FUNCTION, 0, 0, (uint8_t)ret_type, name);
		}

		void set_ret(uint8_t reg_num)
		{
			emit(OPTYPE_SETRET, 0, reg_num);
		}

		void call_native(StringID name)
		{
			emit(OPTYPE_NATIVE, 0, 0, 0, name);
		}

		void push(uint8_t reg_num)
		{
			emit(OPTYPE_PUSH, reg_num);
		}

		void mov(uint8_t reg_num, OPCODE_VARIABLE_TYPE var_type, uint32_t value)
		{
			emit(OPTYPE_LITERAL, reg_num, (uint8_t)var_type, 0, value);
		}

		void ret()
		{
			emit(OPTYPE_RETURN);
		}

		void endfunc()
		{
			emit(OPTYPE_ENDFUNCTION);
		}

	private:
		size_t cur_;
	};
}}

}
