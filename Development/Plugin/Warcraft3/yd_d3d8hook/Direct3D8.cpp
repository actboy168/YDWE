#include "Direct3D8.h"
#include "Direct3DDevice8.h"

CDirect3D8::CDirect3D8(IDirect3D8* d3d)
	: m_d3d(d3d)
{
}

CDirect3D8::~CDirect3D8()
{
}

/*** IUnknown methods ***/
HRESULT WINAPI CDirect3D8::QueryInterface(THIS_ REFIID riid, void** ppvObj)
{
	return m_d3d->QueryInterface(riid, ppvObj);
}

ULONG WINAPI CDirect3D8::AddRef(THIS)
{
	return m_d3d->AddRef();
}

ULONG WINAPI CDirect3D8::Release(THIS)
{
	ULONG count = m_d3d->Release();
	if (0 == count)
	{
		delete this;
	}
	return count;
}

/*** IDirect3D8 methods ***/
HRESULT WINAPI CDirect3D8::RegisterSoftwareDevice(THIS_ void* pInitializeFunction)
{
	return m_d3d->RegisterSoftwareDevice(pInitializeFunction);
}

UINT  WINAPI CDirect3D8::GetAdapterCount(THIS)
{
	return m_d3d->GetAdapterCount();
}

HRESULT WINAPI CDirect3D8::GetAdapterIdentifier(THIS_ UINT Adapter, DWORD Flags, D3DADAPTER_IDENTIFIER8* pIdentifier)
{
	return m_d3d->GetAdapterIdentifier(Adapter, Flags, pIdentifier);
}

UINT WINAPI CDirect3D8::GetAdapterModeCount(THIS_ UINT Adapter)
{
	return m_d3d->GetAdapterModeCount(Adapter);
}

HRESULT WINAPI CDirect3D8::EnumAdapterModes(THIS_ UINT Adapter, UINT Mode, D3DDISPLAYMODE* pMode)
{
	return m_d3d->EnumAdapterModes(Adapter, Mode, pMode);
}


HRESULT WINAPI CDirect3D8::GetAdapterDisplayMode(THIS_ UINT Adapter, D3DDISPLAYMODE* pMode)
{
	return m_d3d->GetAdapterDisplayMode(Adapter, pMode);
}

HRESULT WINAPI CDirect3D8::CheckDeviceType(THIS_ UINT Adapter, D3DDEVTYPE CheckType, D3DFORMAT DisplayFormat, D3DFORMAT BackBufferFormat, BOOL Windowed)
{
	return m_d3d->CheckDeviceType(Adapter, CheckType, DisplayFormat, BackBufferFormat, Windowed);
}

HRESULT WINAPI CDirect3D8::CheckDeviceFormat(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, DWORD Usage, D3DRESOURCETYPE RType, D3DFORMAT CheckFormat)
{
	return m_d3d->CheckDeviceFormat(Adapter, DeviceType, AdapterFormat, Usage, RType, CheckFormat);
}

HRESULT WINAPI CDirect3D8::CheckDeviceMultiSampleType(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT SurfaceFormat, BOOL Windowed, D3DMULTISAMPLE_TYPE MultiSampleType)
{
	return m_d3d->CheckDeviceMultiSampleType(Adapter, DeviceType, SurfaceFormat, Windowed, MultiSampleType);
}

HRESULT WINAPI CDirect3D8::CheckDepthStencilMatch(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DFORMAT AdapterFormat, D3DFORMAT RenderTargetFormat, D3DFORMAT DepthStencilFormat)
{
	return m_d3d->CheckDepthStencilMatch(Adapter, DeviceType, AdapterFormat, RenderTargetFormat, DepthStencilFormat);
}

HRESULT WINAPI CDirect3D8::GetDeviceCaps(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, D3DCAPS8* pCaps)
{
	return m_d3d->GetDeviceCaps(Adapter, DeviceType, pCaps);
}

HMONITOR WINAPI CDirect3D8::GetAdapterMonitor(THIS_ UINT Adapter)
{
	return m_d3d->GetAdapterMonitor(Adapter);
}

HRESULT WINAPI CDirect3D8::CreateDevice(THIS_ UINT Adapter, D3DDEVTYPE DeviceType, HWND hFocusWindow, DWORD BehaviorFlags, D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DDevice8** ppReturnedDeviceInterface)
{
	HRESULT hr = m_d3d->CreateDevice(Adapter, DeviceType, hFocusWindow, BehaviorFlags, pPresentationParameters, ppReturnedDeviceInterface);
	if (SUCCEEDED(hr))
	{
		*ppReturnedDeviceInterface = new CDirect3DDevice8(this, hFocusWindow, *ppReturnedDeviceInterface, pPresentationParameters);
	}
	return hr;
}
