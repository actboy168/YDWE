#pragma once

#include <functional>
#include <memory>
#pragma warning(push)
#pragma warning(disable:6387)
#include <Shlobj.h>
#pragma warning(pop)

namespace base { namespace com { namespace detail {
	template<class _Ty>
	struct unique_ptr_delete
	{
		typedef unique_ptr_delete<_Ty> _Myt;

		unique_ptr_delete()
		{ }

		template<class _Ty2>
		unique_ptr_delete(const unique_ptr_delete<_Ty2>&)
		{ }

		void operator()(_Ty* _Ptr) const
		{
			if(_Ptr) _Ptr->Release();
		}
	};
}

template <class Interface, const IID* interface_id = &__uuidof(Interface)>
class unique_ptr : public std::unique_ptr<Interface, detail::unique_ptr_delete<Interface>>
{   
	typedef std::unique_ptr<Interface, detail::unique_ptr_delete<Interface>> mybase;

public:
	unique_ptr()
		: mybase()
	{ }

	unique_ptr(Interface* ptr)
		: mybase(ptr)
	{ }

	HRESULT Create(std::function<HRESULT(Interface**)> func) 
	{
		Interface* val_ptr;
		HRESULT hr = func(&val_ptr);
		if (SUCCEEDED(hr))
		{
			reset(val_ptr);
		}
		return hr;
	}

	HRESULT CreateInstance(const CLSID& clsid, IUnknown* outer = NULL, DWORD context = CLSCTX_ALL) 
	{
		Interface* val_ptr;
		HRESULT hr = ::CoCreateInstance(clsid, outer, context, *interface_id, reinterpret_cast<void**>(&val_ptr));
		if (SUCCEEDED(hr))
		{
			reset(val_ptr);
		}
		return hr;
	}

	template <class Query>
	HRESULT QueryInterface(Query** p) 
	{
		return get()->QueryInterface(p);
	}

	HRESULT QueryInterface(const IID& iid, void** obj) 
	{
		return get()->QueryInterface(iid, obj);
	}

	HRESULT QueryFrom(IUnknown* object) 
	{
		Interface* val_ptr;
		HRESULT hr = object->QueryInterface(&val_ptr);
		if (SUCCEEDED(hr))
		{
			reset(val_ptr);
		}
		return hr;
	}
};
}}
