#pragma warning(push, 3)
#include <lua.hpp>
#include <luabind/luabind.hpp>
#include <base/lua/luabind.h>
#pragma warning(pop)				 	  		
#include <base/filesystem.h>

#include <windows.h>
#include <mscoree.h>
#include <metahost.h>
#import "mscorlib.tlb" raw_interfaces_only, rename("ReportEvent","ReportCLREvent")
#include <atlcomcli.h>

#if _MSC_VER >= 1800
#	pragma comment(lib, "atls.lib")
#else
#	if !defined(_DEBUG)
#		pragma comment(lib, "atls.lib")
#	else
#		pragma comment(lib, "atlsd.lib")
#	endif
#endif

namespace clr_helper { namespace runtime {

	typedef HRESULT __stdcall  PROC_CLRCreateInstance(
		REFCLSID  clsid,
		REFIID     riid,
		LPVOID  * ppInterface
		);

	typedef HRESULT __stdcall PROC_CorBindToRuntime(
		LPCWSTR  pwszVersion,
		LPCWSTR  pwszBuildFlavor,
		REFCLSID rclsid,
		REFIID   riid,
		LPVOID*  ppv);

	typedef HRESULT __stdcall PROC_CorBindToRuntimeEx(
		LPCWSTR  pwszVersion,
		LPCWSTR  pwszBuildFlavor,
		DWORD    startupFlags,
		REFCLSID rclsid,
		REFIID   riid,
		LPVOID*  ppv);

	uintptr_t FuncCLRCreateInstance  = 0;
	uintptr_t FuncCorBindToRuntimeEx = 0;
	uintptr_t FuncCorBindToRuntime   = 0;

	void enumerate(ICLRMetaHost* meta_host, std::function<bool(ICLRRuntimeInfo*)> Func)
	{
		CComPtr<IEnumUnknown> runtime_enumerator;
		HRESULT hr = meta_host->EnumerateInstalledRuntimes(&runtime_enumerator);
		if (!SUCCEEDED(hr)) 
		{
			return ;
		}

		for (CComPtr<IUnknown> it = nullptr; runtime_enumerator->Next(1, &it, NULL) == S_OK; it.Release())
		{
			CComPtr<ICLRRuntimeInfo> runtime_info;
			if (SUCCEEDED(it.QueryInterface(&runtime_info)))
			{
				if (!Func(runtime_info))
				{
					return ;
				}
			}
		}

		return;
	}

	bool is_loadable(ICLRRuntimeInfo* ptr)
	{
		BOOL loadable = FALSE;
		if (!SUCCEEDED( ptr->IsLoadable(&loadable)))
		{
			return false;
		}

		return !!loadable;
	}

	bool read_version_string(ICLRRuntimeInfo* ptr, std::wstring& str)
	{
		str.resize(64);
		DWORD cch = str.size();
		if (!SUCCEEDED(ptr->GetVersionString(const_cast<wchar_t*>(str.data()), &cch)))
		{
			return false;
		}

		return true;
	}

	ICLRRuntimeInfo* get_latest(ICLRMetaHost* meta_host)
	{
		ICLRRuntimeInfo* latest_ptr = nullptr;
		std::wstring latest_runtime_version;
		std::wstring current_runtime_version;

		enumerate(meta_host, [&](ICLRRuntimeInfo* rt_ptr)->bool 
		{
			if (!is_loadable(rt_ptr))
			{
				return true;
			}

			if (!latest_ptr)
			{
				if (SUCCEEDED(rt_ptr->QueryInterface(IID_PPV_ARGS(&latest_ptr))))
				{
					read_version_string(latest_ptr, latest_runtime_version);
				}
			}
			else
			{
				if (read_version_string(rt_ptr, current_runtime_version))
				{
					if (latest_runtime_version < current_runtime_version)
					{
						ICLRRuntimeInfo* tmp = nullptr;
						if (SUCCEEDED(rt_ptr->QueryInterface(IID_PPV_ARGS(&tmp))))
						{
							latest_runtime_version = current_runtime_version;
							latest_ptr->Release();
							latest_ptr = tmp;
						}
					}
				}
			}

			return true;
		});

		return latest_ptr;
	}

	HRESULT create_host_4_0(const wchar_t* version, ICorRuntimeHost** ptr)
	{
		HRESULT hr = S_OK;

		CComPtr<ICLRMetaHost> meta_host;
		hr = ((PROC_CLRCreateInstance*)FuncCLRCreateInstance)(CLSID_CLRMetaHost, IID_PPV_ARGS(&meta_host));
		if (!SUCCEEDED(hr))
		{
			return hr;
		}

		if (version)
		{
			CComPtr<ICLRRuntimeInfo> runtime_info = nullptr;
			hr = meta_host->GetRuntime(version, IID_PPV_ARGS(&runtime_info));
			if (!SUCCEEDED(hr))
			{
				return hr;
			}

			if (!is_loadable(runtime_info))
			{
				return E_FAIL;
			}

			return runtime_info->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(ptr));
		}
		else
		{
			CComPtr<ICLRRuntimeInfo> runtime_info(get_latest(meta_host));

			if (!runtime_info)
			{
				return E_FAIL;
			}

			return runtime_info->GetInterface(CLSID_CorRuntimeHost, IID_PPV_ARGS(ptr));
		}
	}

	HRESULT create_host_2_0(const wchar_t* version, ICorRuntimeHost** ptr)
	{
		return ((PROC_CorBindToRuntimeEx*)FuncCorBindToRuntimeEx)(
			version,
			L"wks",
			(STARTUP_LOADER_OPTIMIZATION_SINGLE_DOMAIN | STARTUP_CONCURRENT_GC),
			CLSID_CorRuntimeHost,
			IID_PPV_ARGS(ptr));
	}

	HRESULT create_host_1_0(const wchar_t* version, ICorRuntimeHost** ptr)
	{
		return ((PROC_CorBindToRuntime*)FuncCorBindToRuntime)(
			version,
			L"wks",
			CLSID_CorRuntimeHost,
			IID_PPV_ARGS(ptr));
	}

	HRESULT create_host(const wchar_t* version, ICorRuntimeHost** ptr)
	{
		HMODULE mscoree_handle = ::LoadLibraryW(L"mscoree.dll");
		FuncCLRCreateInstance  = (uintptr_t)::GetProcAddress(mscoree_handle, "CLRCreateInstance");  // .NET 4.0+
		FuncCorBindToRuntimeEx = (uintptr_t)::GetProcAddress(mscoree_handle, "CorBindToRuntimeEx"); // .NET 2.0/3.5
		FuncCorBindToRuntime   = (uintptr_t)::GetProcAddress(mscoree_handle, "CorBindToRuntime");   // .NET 1.0/1.1

		if (FuncCLRCreateInstance)
		{ 
			return create_host_4_0(version, ptr);
		}
		else if (FuncCorBindToRuntimeEx)
		{
			return create_host_2_0(version, ptr);
		}
		else if (FuncCorBindToRuntime)
		{
			return create_host_1_0(version, ptr);
		}

		return E_FAIL;
	}

	mscorlib::_AppDomain* default_domain(CComPtr<ICorRuntimeHost>& host)
	{
		CComPtr<IUnknown> unknown_appdomain;
		CComPtr<mscorlib::_AppDomain> appdomain;
		if (!SUCCEEDED(host->GetDefaultDomain(&unknown_appdomain)))
		{
			return nullptr;
		}
		if (!SUCCEEDED(unknown_appdomain.QueryInterface(&appdomain)))
		{
			return nullptr;
		}
		return appdomain.Detach();
	}

	mscorlib::_AppDomain* create_appdomain(const wchar_t* version)
	{
		CComPtr<ICorRuntimeHost> host;
		HRESULT hr = create_host(version, &host);
		if (!SUCCEEDED(hr))
		{
			return nullptr;
		}

		if (!SUCCEEDED(host->Start()))
		{
			return nullptr;
		}

		return default_domain(host);
	}
}

	class object
	{
	public:
		object(mscorlib::_AppDomain* appdomain, const wchar_t* assembly, const wchar_t* type)
			: vt_()
			, handle_()
			, object_()
			, type_()
			, vaild_(false)
			, last_code_(S_OK)
		{
			last_code_ = create(appdomain, assembly, type);
		}

		HRESULT create(mscorlib::_AppDomain* appdomain, const wchar_t* assembly, const wchar_t* type)
		{
			HRESULT hr = S_OK;

			if (!appdomain)
			{
				return E_FAIL;
			}

			handle_.Release();
			object_.Release();
			type_.Release();

			BSTR bstr_assembly = SysAllocString(assembly);
			BSTR bstr_type = SysAllocString(type);
			hr = appdomain->CreateInstanceFrom(bstr_assembly, bstr_type, &handle_);
			SysFreeString(bstr_assembly);
			SysFreeString(bstr_type);

			if (!SUCCEEDED(hr))
			{
				return hr;
			}

			hr = handle_->Unwrap(&vt_);
			if (!SUCCEEDED(hr))
			{
				return hr;
			}

			object_.Attach((mscorlib::_Object *)vt_.punkVal);

			hr = object_->GetType(&type_);
			if (!SUCCEEDED(hr))
			{
				return hr;			
			}

			vaild_ = true;
			return hr;
		}

		HRESULT call(const wchar_t* name)
		{
			if (!vaild_)
			{
				return E_FAIL;
			}

			BSTR bstr_name = SysAllocString(name);
			HRESULT hr = type_->InvokeMember_3(
				bstr_name,
				mscorlib::BindingFlags_InvokeMethod,
				NULL, 
				vt_, 
				NULL, 
				NULL);
			SysFreeString(bstr_name);

			return hr;
		}

		uint32_t error_code() const
		{
			return last_code_;
		}

	private:
		variant_t                        vt_;
		CComPtr<mscorlib::_ObjectHandle> handle_;
		CComPtr<mscorlib::_Object>       object_;
		CComPtr<mscorlib::_Type>         type_;
		bool                             vaild_;
		HRESULT                          last_code_;
	};
}

namespace NLua { namespace CLR {

	class AppDomain
	{
	public:
		AppDomain()
			: ptr_(clr_helper::runtime::create_appdomain(nullptr))
		{ }

		AppDomain(std::wstring const& version)
			: ptr_(clr_helper::runtime::create_appdomain(version.c_str()))
		{ }

		mscorlib::_AppDomain const* get() const
		{
			return ptr_;
		}

		mscorlib::_AppDomain* get()
		{
			return ptr_;
		}

		bool vaild() const
		{
			return !!ptr_;
		}

	private:
		CComPtr<mscorlib::_AppDomain> ptr_;
	};

	class Object
	{
	public:
		Object(AppDomain& appdomain, std::wstring const& assembly, std::wstring const& type)
			: object_(appdomain.get(), assembly.c_str(), type.c_str())
		{ }

		Object(AppDomain& appdomain, fs::path const& assembly, std::wstring const& type)
			: object_(appdomain.get(), assembly.c_str(), type.c_str())
		{ }

		uint32_t call(std::wstring const& name)
		{
			return (uint32_t)(object_.call(name.c_str()));
		}

		uint32_t error_code() const
		{
			return object_.error_code();
		}

	private:
		clr_helper::object object_;
	};
}}

int luaopen_clr(lua_State *pState)
{
	using namespace luabind;

	module(pState, "clr")
	[
		class_<NLua::CLR::AppDomain>("appdomain")
			.def(constructor<>())
			.def(constructor<const std::wstring&>())
			.def("vaild", &NLua::CLR::AppDomain::vaild)
		,
		class_<NLua::CLR::Object>("object")
			.def(constructor<NLua::CLR::AppDomain&, std::wstring const&, std::wstring const&>())
			.def(constructor<NLua::CLR::AppDomain&, fs::path const&, std::wstring const&>())
			.def("call",       &NLua::CLR::Object::call)
			.def("error_code", &NLua::CLR::Object::error_code)
	];

	return 0;
}
