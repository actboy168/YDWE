#pragma once

#include <windows.h>
#include <deque>
#include <aero/function/fp_call.hpp>

class base_module
{
public:
	base_module(const wchar_t* module_name)
		: module_(::LoadLibraryW(module_name))
	{ }

	virtual ~base_module()
	{
		release();
	}

	bool valid() const
	{
		return !!module_;
	}

	void release()
	{
		if (valid())
		{
			::FreeLibrary(module_);
			module_ = NULL;
		}
	}

	uintptr_t proc(uint32_t ord)
	{
		return valid() ? (uintptr_t)::GetProcAddress(module_, (const char*)ord) : 0;
	}

private:
	HMODULE   module_;
};

class storm_module : public base_module
{
public:
	storm_module() 
		: base_module(L"Storm.dll")
	{
		if (!base_module::valid()) throw std::exception("can't find storm.dll.");
		if (!(SFileLoadFile_	 = base_module::proc(279))) std::exception("can't find storm.dll!279.");
		if (!(SFileUnloadFile_	 = base_module::proc(280))) std::exception("can't find storm.dll!280.");
	}

protected:
	bool load(const char* file_path, const uint8_t** buffer_ptr, uint32_t* size_ptr, uint32_t reserve_size, uint32_t search_scope)
	{
		return !!aero::std_call<BOOL>(SFileLoadFile_, file_path, buffer_ptr, size_ptr, reserve_size, search_scope);
	}

	bool unload(const uint8_t* buffer)
	{
		return !!aero::std_call<BOOL>(SFileUnloadFile_, buffer);
	}

private:
	uintptr_t SFileLoadFile_;
	uintptr_t SFileUnloadFile_;
};

class storm : public storm_module
{
public:
	std::string load(const char* file_path)
	{
		const uint8_t* buffer  = nullptr;
		uint32_t       size = 0;
		if (!storm_module::load(file_path, &buffer, &size, 0, 0))
		{
			throw std::exception("open mpq file failed.");
		}

		std::string tmp((const char*)buffer, size);
		storm_module::unload(buffer);
		return std::move(tmp);
	}
};
