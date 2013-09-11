#include <base/exception/seh_exception.h>
#include <base/exception/windows_exception.h>

namespace base {

	seh_exception::seh_exception(const PEXCEPTION_POINTERS exp_ptr)
		: windows_exception("seh exception", exp_ptr->ExceptionRecord->ExceptionCode)
		, exception_record_(*exp_ptr->ExceptionRecord)
		, context_record_(*exp_ptr->ContextRecord)
	{ }

	const EXCEPTION_RECORD& seh_exception::get_exception_record() const throw()
	{
		return exception_record_;
	}

	const CONTEXT& seh_exception::get_context() const throw()
	{
		return context_record_;
	}

	void seh_exception::trans_func(unsigned int /*u*/, EXCEPTION_POINTERS* exp_ptr)
	{
		throw seh_exception(exp_ptr);
	}

	void seh_exception::initialize()
	{
#pragma warning(push)
#pragma warning(disable: 4535)
		_set_se_translator(trans_func);
#pragma warning(pop)
	}
}
