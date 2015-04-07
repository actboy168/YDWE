#pragma once

namespace base {
	template <class OutputClass, class InputClass>
	union horrible_union 
	{
		OutputClass out;
		InputClass in;
	};
	
	template <class OutputClass, class InputClass>
	inline OutputClass horrible_cast(const InputClass input)
	{
		horrible_union<OutputClass, InputClass> u;
		static_assert((sizeof(InputClass)==sizeof(u)) && (sizeof(InputClass)==sizeof(OutputClass)), "OutputClass and InputClass are not the same size.");
		u.in = input;
		return u.out;
	}
}
