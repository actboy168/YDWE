#define _CRT_SECURE_NO_WARNINGS
#include <base/warcraft3/jass/opcode.h>
#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace jass {

	const char* get_typename(uint8_t t) {
		switch (t) {
		case OPCODE_VARIABLE_NOTHING:
			return "nothing";
		case OPCODE_VARIABLE_UNKNOWN:
			return "unknown";
		case OPCODE_VARIABLE_NULL:
			return "null";
		case OPCODE_VARIABLE_CODE:
			return "code";
		case OPCODE_VARIABLE_INTEGER:
			return "integer";
		case OPCODE_VARIABLE_REAL:
			return "real";
		case OPCODE_VARIABLE_STRING:
			return "string";
		case OPCODE_VARIABLE_HANDLE:
			return "handle";
		case OPCODE_VARIABLE_BOOLEAN:
			return "boolean";
		case OPCODE_VARIABLE_INTEGER_ARRAY:
			return "integer array";
		case OPCODE_VARIABLE_REAL_ARRAY:
			return "real array";
		case OPCODE_VARIABLE_STRING_ARRAY:
			return "string array";
		case OPCODE_VARIABLE_HANDLE_ARRAY:
			return "handle array";
		case OPCODE_VARIABLE_BOOLEAN_ARRAY:
			return "boolean array";
		default:
			return "???";
		}
	}

	void dump_opcode(opcode* op, const wchar_t* filename) {
		FILE* f = _wfopen(filename, L"w");
		if (!f) {
			return;
		}
		for (; op->op != OPTYPE_ENDPROGRAM; ++op) {
			switch (op->op) {
			case OPTYPE_FUNCTION:
				fprintf(f, "func %s\n", from_stringid(op->arg));
				break;
			case OPTYPE_ENDFUNCTION:
				fprintf(f, "endfunc\n");
				break;
			case OPTYPE_LOCAL:
				fprintf(f, "local %s: %s\n", get_typename(op->r1), from_stringid(op->arg));
				break;
			case OPTYPE_GLOBAL:
				fprintf(f, "global %s: %s\n", get_typename(op->r1), from_stringid(op->arg));
				break;
			case OPTYPE_CONSTANT:
				fprintf(f, "const %s: %s\n", get_typename(op->r1), from_stringid(op->arg));
				break;
			case OPTYPE_FUNCARG:
				fprintf(f, "arg %d, %s: %s\n", op->r2, get_typename(op->r1), from_stringid(op->arg));
				break;
			case OPTYPE_EXTENDS:
				fprintf(f, "extends %s\n", from_stringid(op->arg));
				break;
			case OPTYPE_TYPE:
				fprintf(f, "type %s\n", from_stringid(op->arg));
				break;
			case OPTYPE_POPN:
				fprintf(f, "popn %d\n", op->r1);
				break;
			case OPTYPE_MOVRLITERAL:
				switch (op->r2) {
				case OPCODE_VARIABLE_NOTHING:
					fprintf(f, "mov r%02X, nothing\n", op->r1);
					break;
				case OPCODE_VARIABLE_NULL:
					fprintf(f, "mov r%02X, null\n", op->r1);
					break;
				case OPCODE_VARIABLE_INTEGER:
					fprintf(f, "mov r%02X, %d\n", op->r1, op->arg);
					break;
				case OPCODE_VARIABLE_REAL:
					fprintf(f, "mov r%02X, %ff\n", op->r1, from_real(op->arg));
					break;
				case OPCODE_VARIABLE_STRING:
					if (op->arg == 0) {
						fprintf(f, "mov r%02X, string: null\n", op->r1);
					}
					else {
						fprintf(f, "mov r%02X, \"%s\"\n", op->r1, from_stringid(op->arg));
					}
					break;
				case OPCODE_VARIABLE_HANDLE:
					fprintf(f, "mov r%02X, handle: %08x\n", op->r1, op->arg);
					break;
				case OPCODE_VARIABLE_BOOLEAN:
					fprintf(f, "mov r%02X, %s\n", op->r1, op->arg ? "true" : "false");
					break;
				default:
					fprintf(f, "mov r%02X, %s: %08x\n", op->r1, get_typename(op->r2), op->arg);
					break;
				}
				break;
			case OPTYPE_MOVRR:
				fprintf(f, "mov r%02X, r%02X\n", op->r1, op->r2);
				break;
			case OPTYPE_MOVRV:
				fprintf(f, "mov r%02X, %s: %s\n", op->r1, get_typename(op->r2), from_stringid(op->arg));
				break;
			case OPTYPE_MOVRCODE:
				fprintf(f, "mov r%02X, %s: %s\n", op->r1, get_typename(op->r2), from_stringid(op->arg));
				break;
			case OPTYPE_MOVRA:
				fprintf(f, "mov r%02X, %s: %s[r%02X]\n", op->r1, get_typename(op->r3), from_stringid(op->arg), op->r2);
				break;
			case OPTYPE_MOVVR:
				fprintf(f, "mov %s, r%02X\n", from_stringid(op->arg), op->r1);
				break;
			case OPTYPE_MOVAR:
				fprintf(f, "mov %s[r%02X], r%02X\n", from_stringid(op->arg), op->r1, op->r2);
				break;
			case OPTYPE_PUSH:
				fprintf(f, "push r%02X\n", op->r1);
				break;
			case OPTYPE_POP:
				fprintf(f, "pop r%02X\n", op->r1);
				break;
			case OPTYPE_CALLNATIVE:
				fprintf(f, "call %s\n", from_stringid(op->arg));
				break;
			case OPTYPE_CALLJASS:
				fprintf(f, "call %s\n", from_stringid(op->arg));
				break;
			case OPTYPE_I2R:
				fprintf(f, "i2r r%02X\n", op->r1);
				break;
			case OPTYPE_AND:
				fprintf(f, "and r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_OR:
				fprintf(f, "or r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_EQUAL:
				fprintf(f, "eq r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_NOTEQUAL:
				fprintf(f, "neq r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_LESSEREQUAL:
				fprintf(f, "le r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_GREATEREQUAL:
				fprintf(f, "ge r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_LESSER:
				fprintf(f, "lt r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_GREATER:
				fprintf(f, "gt r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_ADD:
				fprintf(f, "add r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_SUB:
				fprintf(f, "sub r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_MUL:
				fprintf(f, "mul r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_DIV:
				fprintf(f, "div r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_MOD:
				fprintf(f, "mod r%02X, r%02X, r%02X\n", op->r1, op->r2, op->r3);
				break;
			case OPTYPE_NEGATE:
				fprintf(f, "neg r%02X\n", op->r1);
				break;
			case OPTYPE_NOT:
				fprintf(f, "not r%02X\n", op->r1);
				break;
			case OPTYPE_RETURN:
				fprintf(f, "return\n");
				break;
			case OPTYPE_LABEL:
				fprintf(f, "label %08x\n", op->arg);
				break;
			case OPTYPE_JUMPIFTRUE:
				fprintf(f, "jt r%02X, %08x\n", op->r1, op->arg);
				break;
			case OPTYPE_JUMPIFFALSE:
				fprintf(f, "jf r%02X, %08x\n", op->r1, op->arg);
				break;
			case OPTYPE_JUMP:
				fprintf(f, "jmp %08x\n", op->arg);
				break;
			}
		}
		fclose(f);
	}
}}}
