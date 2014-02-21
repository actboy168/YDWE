#pragma once

#include <base/config.h>
#include <base/exception/windows_exception.h>
#include <Windows.h>

namespace base { 

class _BASE_API seh_exception : public windows_exception
{
public:
	seh_exception(const PEXCEPTION_POINTERS exp_ptr);
	virtual const EXCEPTION_RECORD& get_exception_record() const throw();
	virtual const CONTEXT& get_context() const throw();
	static void trans_func(unsigned int /*u*/, EXCEPTION_POINTERS* exp_ptr);
	static void initialize();

protected:
	EXCEPTION_RECORD exception_record_;
	CONTEXT context_record_;
};

}
