#include <base/warcraft3/jass.h>

namespace base { namespace warcraft3 { namespace jass {
	/*
	Jass中实数类型转换。
	为了防止编译器的优化，必须把编译选项中的“浮点模型”改为“严格”。
	*/
	float from_real(jreal_t val)
	{
		return *(float*)&val;
	}

	jreal_t to_real(float val)
	{
		return *(jreal_t*)&val;
	}
}}}
