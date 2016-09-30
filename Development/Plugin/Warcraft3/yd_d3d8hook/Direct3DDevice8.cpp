#include "Direct3DDevice8.h"

bool gamerender = false;

CDirect3DDevice8::CDirect3DDevice8(IDirect3D8* d3d, HWND hwnd, IDirect3DDevice8* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	: m_d3d(d3d)
	, m_device(device)
{
}

CDirect3DDevice8::~CDirect3DDevice8()
{
}

HRESULT WINAPI CDirect3DDevice8::QueryInterface(THIS_ REFIID riid, void** ppvObj)
{
	return m_device->QueryInterface(riid, ppvObj);
}

ULONG WINAPI CDirect3DDevice8::AddRef(THIS)
{
	return m_device->AddRef();
}

ULONG WINAPI CDirect3DDevice8::Release(THIS)
{
	DWORD refCount = m_device->Release();
	if (0 == refCount)
	{
		delete this;
	}
	return refCount;
}

/*** IDirect3DDevice8 methods ***/
HRESULT WINAPI CDirect3DDevice8::TestCooperativeLevel(THIS)
{
	return m_device->TestCooperativeLevel();
}


UINT WINAPI CDirect3DDevice8::GetAvailableTextureMem(THIS)
{
	return m_device->GetAvailableTextureMem();
}

HRESULT WINAPI CDirect3DDevice8::ResourceManagerDiscardBytes(THIS_ DWORD Bytes)
{
	return m_device->ResourceManagerDiscardBytes(Bytes);
}

HRESULT WINAPI CDirect3DDevice8::GetDirect3D(THIS_ IDirect3D8** ppD3D8)
{
	HRESULT hRet = m_device->GetDirect3D(ppD3D8);
	if (SUCCEEDED(hRet))
		*ppD3D8 = m_d3d;
	return hRet;
}

HRESULT WINAPI CDirect3DDevice8::GetDeviceCaps(THIS_ D3DCAPS8* pCaps)
{
	return m_device->GetDeviceCaps(pCaps);
}

HRESULT WINAPI CDirect3DDevice8::GetDisplayMode(THIS_ D3DDISPLAYMODE* pMode)
{
	return m_device->GetDisplayMode(pMode);
}

HRESULT WINAPI CDirect3DDevice8::GetCreationParameters(THIS_ D3DDEVICE_CREATION_PARAMETERS *pParameters)
{
	return m_device->GetCreationParameters(pParameters);
}

HRESULT CDirect3DDevice8::SetCursorProperties(THIS_ UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8* pCursorBitmap)
{
	return m_device->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

void WINAPI CDirect3DDevice8::SetCursorPosition(THIS_ int X, int Y, DWORD Flags)
{
	m_device->SetCursorPosition(X, Y, Flags);
}

BOOL WINAPI CDirect3DDevice8::ShowCursor(THIS_ BOOL bShow)
{
	return m_device->ShowCursor(bShow);
}

HRESULT WINAPI CDirect3DDevice8::CreateAdditionalSwapChain(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain8** pSwapChain)
{
	return m_device->CreateAdditionalSwapChain(pPresentationParameters, pSwapChain);
}

HRESULT WINAPI CDirect3DDevice8::Reset(THIS_ D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	return m_device->Reset(pPresentationParameters);
}

HRESULT CDirect3DDevice8::Present(THIS_ CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	return m_device->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}

HRESULT CDirect3DDevice8::GetBackBuffer(THIS_ UINT BackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface8** ppBackBuffer)
{
	return m_device->GetBackBuffer(BackBuffer, Type, ppBackBuffer);
}

HRESULT CDirect3DDevice8::GetRasterStatus(THIS_ D3DRASTER_STATUS* pRasterStatus)
{
	return m_device->GetRasterStatus(pRasterStatus);
}

void WINAPI CDirect3DDevice8::SetGammaRamp(THIS_ DWORD Flags, CONST D3DGAMMARAMP* pRamp)
{
	m_device->SetGammaRamp(Flags, pRamp);
}

void WINAPI CDirect3DDevice8::GetGammaRamp(THIS_ D3DGAMMARAMP* pRamp)
{
	m_device->GetGammaRamp(pRamp);
}

HRESULT WINAPI CDirect3DDevice8::CreateTexture(THIS_ UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture8** ppTexture)
{
	return m_device->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture);
}

HRESULT WINAPI CDirect3DDevice8::CreateVolumeTexture(THIS_ UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture8** ppVolumeTexture)
{
	return m_device->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture);
}

HRESULT WINAPI CDirect3DDevice8::CreateCubeTexture(THIS_ UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture8** ppCubeTexture)
{
	return m_device->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture);
}

HRESULT WINAPI CDirect3DDevice8::CreateVertexBuffer(THIS_ UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer8** ppVertexBuffer)
{
	return m_device->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer);
}

HRESULT WINAPI CDirect3DDevice8::CreateIndexBuffer(THIS_ UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer8** ppIndexBuffer)
{
	return m_device->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer);
}

HRESULT WINAPI CDirect3DDevice8::CreateRenderTarget(THIS_ UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, BOOL Lockable, IDirect3DSurface8** ppSurface)
{
	return m_device->CreateRenderTarget(Width, Height, Format, MultiSample, Lockable, ppSurface);
}

HRESULT WINAPI CDirect3DDevice8::CreateDepthStencilSurface(THIS_ UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, IDirect3DSurface8** ppSurface)
{
	return m_device->CreateDepthStencilSurface(Width, Height, Format, MultiSample, ppSurface);
}

HRESULT WINAPI CDirect3DDevice8::CreateImageSurface(THIS_ UINT Width, UINT Height, D3DFORMAT Format, IDirect3DSurface8** ppSurface)
{
	return m_device->CreateImageSurface(Width, Height, Format, ppSurface);
}

HRESULT WINAPI CDirect3DDevice8::CopyRects(THIS_ IDirect3DSurface8* pSourceSurface, CONST RECT* pSourceRectsArray, UINT cRects, IDirect3DSurface8* pDestinationSurface, CONST POINT* pDestPointsArray)
{
	return m_device->CopyRects(pSourceSurface, pSourceRectsArray, cRects, pDestinationSurface, pDestPointsArray);
}

HRESULT WINAPI CDirect3DDevice8::UpdateTexture(THIS_ IDirect3DBaseTexture8* pSourceTexture, IDirect3DBaseTexture8* pDestinationTexture)
{
	return m_device->UpdateTexture(pSourceTexture, pDestinationTexture);
}

HRESULT WINAPI CDirect3DDevice8::GetFrontBuffer(THIS_ IDirect3DSurface8* pDestSurface)
{
	return m_device->GetFrontBuffer(pDestSurface);
}

HRESULT WINAPI CDirect3DDevice8::SetRenderTarget(THIS_ IDirect3DSurface8* pRenderTarget, IDirect3DSurface8* pNewZStencil)
{
	return m_device->SetRenderTarget(pRenderTarget, pNewZStencil);
}

HRESULT WINAPI CDirect3DDevice8::GetRenderTarget(THIS_ IDirect3DSurface8** ppRenderTarget)
{
	return m_device->GetRenderTarget(ppRenderTarget);
}

HRESULT WINAPI CDirect3DDevice8::GetDepthStencilSurface(THIS_ IDirect3DSurface8** ppZStencilSurface)
{
	return m_device->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT WINAPI CDirect3DDevice8::BeginScene(THIS)
{
	return m_device->BeginScene();
}

HRESULT WINAPI CDirect3DDevice8::EndScene(THIS)
{
	return m_device->EndScene();
}

HRESULT WINAPI CDirect3DDevice8::Clear(THIS_ DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	return m_device->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT WINAPI CDirect3DDevice8::SetTransform(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	if (State == D3DTS_PROJECTION)
	{
		D3DMATRIX* pMatrixNew = new D3DMATRIX(*pMatrix);
		if (gamerender)
		{
			//pMatrixNew->m[1][1] *= 4.0f/3.0f;//vertical FoV
			pMatrixNew->m[0][0] /= 4.0f / 3.0f;// horizontal Fov
		}
		return (m_device->SetTransform(State, pMatrixNew));
	}
	return m_device->SetTransform(State, pMatrix);
}

HRESULT WINAPI CDirect3DDevice8::GetTransform(THIS_ D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix)
{
	return m_device->GetTransform(State, pMatrix);
}

HRESULT WINAPI CDirect3DDevice8::MultiplyTransform(THIS_ D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix)
{
	return m_device->MultiplyTransform(State, pMatrix);
}

HRESULT WINAPI CDirect3DDevice8::SetViewport(THIS_ CONST D3DVIEWPORT8* pViewport)
{
	//some shitty constants to determine region where actual game draws
	double rate = (double)pViewport->Y / (double)pViewport->Height;
	gamerender = rate >= 0.02 && rate <= 0.05;
	return m_device->SetViewport(pViewport);
}

HRESULT WINAPI CDirect3DDevice8::GetViewport(THIS_ D3DVIEWPORT8* pViewport)
{
	return m_device->GetViewport(pViewport);
}

HRESULT WINAPI CDirect3DDevice8::SetMaterial(THIS_ CONST D3DMATERIAL8* pMaterial)
{
	return m_device->SetMaterial(pMaterial);
}

HRESULT WINAPI CDirect3DDevice8::GetMaterial(THIS_ D3DMATERIAL8* pMaterial)
{
	return m_device->GetMaterial(pMaterial);
}

HRESULT WINAPI CDirect3DDevice8::SetLight(THIS_ DWORD Index, CONST D3DLIGHT8* pLight)
{
	return m_device->SetLight(Index, pLight);
}

HRESULT WINAPI CDirect3DDevice8::GetLight(THIS_ DWORD Index, D3DLIGHT8* pLight)
{
	return m_device->GetLight(Index, pLight);
}

HRESULT WINAPI CDirect3DDevice8::LightEnable(THIS_ DWORD Index, BOOL Enable)
{
	return m_device->LightEnable(Index, Enable);
}

HRESULT WINAPI CDirect3DDevice8::GetLightEnable(THIS_ DWORD Index, BOOL* pEnable)
{
	return m_device->GetLightEnable(Index, pEnable);
}

HRESULT WINAPI CDirect3DDevice8::SetClipPlane(THIS_ DWORD Index, CONST float* pPlane)
{
	return m_device->SetClipPlane(Index, pPlane);
}

HRESULT WINAPI CDirect3DDevice8::GetClipPlane(THIS_ DWORD Index, float* pPlane)
{
	return m_device->GetClipPlane(Index, pPlane);
}

HRESULT WINAPI CDirect3DDevice8::SetRenderState(THIS_ D3DRENDERSTATETYPE State, DWORD Value)
{
	return m_device->SetRenderState(State, Value);
}

HRESULT WINAPI CDirect3DDevice8::GetRenderState(THIS_ D3DRENDERSTATETYPE State, DWORD* pValue)
{
	return m_device->GetRenderState(State, pValue);
}

HRESULT WINAPI CDirect3DDevice8::BeginStateBlock(THIS)
{
	return m_device->BeginStateBlock();
}

HRESULT WINAPI CDirect3DDevice8::EndStateBlock(THIS_ DWORD* pToken)
{
	return m_device->EndStateBlock(pToken);
}

HRESULT WINAPI CDirect3DDevice8::ApplyStateBlock(THIS_ DWORD Token)
{
	return m_device->ApplyStateBlock(Token);
}

HRESULT WINAPI CDirect3DDevice8::CaptureStateBlock(THIS_ DWORD Token)
{
	return m_device->CaptureStateBlock(Token);
}

HRESULT WINAPI CDirect3DDevice8::DeleteStateBlock(THIS_ DWORD Token)
{
	return m_device->DeleteStateBlock(Token);
}

HRESULT WINAPI CDirect3DDevice8::CreateStateBlock(THIS_ D3DSTATEBLOCKTYPE Type, DWORD* pToken)
{
	return m_device->CreateStateBlock(Type, pToken);
}

HRESULT WINAPI CDirect3DDevice8::SetClipStatus(THIS_ CONST D3DCLIPSTATUS8* pClipStatus)
{
	return m_device->SetClipStatus(pClipStatus);
}

HRESULT WINAPI CDirect3DDevice8::GetClipStatus(THIS_ D3DCLIPSTATUS8* pClipStatus)
{
	return m_device->GetClipStatus(pClipStatus);
}

HRESULT WINAPI CDirect3DDevice8::GetTexture(THIS_ DWORD Stage, IDirect3DBaseTexture8** ppTexture)
{
	return m_device->GetTexture(Stage, ppTexture);
}

HRESULT WINAPI CDirect3DDevice8::SetTexture(THIS_ DWORD Stage, IDirect3DBaseTexture8* pTexture)
{
	return m_device->SetTexture(Stage, pTexture);
}

HRESULT WINAPI CDirect3DDevice8::GetTextureStageState(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue)
{
	return m_device->GetTextureStageState(Stage, Type, pValue);
}

HRESULT WINAPI CDirect3DDevice8::SetTextureStageState(THIS_ DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value)
{
	return m_device->SetTextureStageState(Stage, Type, Value);
}

HRESULT WINAPI CDirect3DDevice8::ValidateDevice(THIS_ DWORD* pNumPasses)
{
	return m_device->ValidateDevice(pNumPasses);
}

HRESULT WINAPI CDirect3DDevice8::GetInfo(THIS_ DWORD DevInfoID, void* pDevInfoStruct, DWORD DevInfoStructSize)
{
	return m_device->GetInfo(DevInfoID, pDevInfoStruct, DevInfoStructSize);
}

HRESULT WINAPI CDirect3DDevice8::SetPaletteEntries(THIS_ UINT PaletteNumber, CONST PALETTEENTRY* pEntries)
{
	return m_device->SetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT WINAPI CDirect3DDevice8::GetPaletteEntries(THIS_ UINT PaletteNumber, PALETTEENTRY* pEntries)
{
	return m_device->GetPaletteEntries(PaletteNumber, pEntries);
}

HRESULT WINAPI CDirect3DDevice8::SetCurrentTexturePalette(THIS_ UINT PaletteNumber)
{
	return m_device->SetCurrentTexturePalette(PaletteNumber);
}

HRESULT WINAPI CDirect3DDevice8::GetCurrentTexturePalette(THIS_ UINT *PaletteNumber)
{
	return m_device->GetCurrentTexturePalette(PaletteNumber);
}

HRESULT WINAPI CDirect3DDevice8::DrawPrimitive(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount)
{
	return m_device->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);
}

HRESULT WINAPI CDirect3DDevice8::DrawIndexedPrimitive(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT minIndex, UINT NumVertices, UINT startIndex, UINT primCount)
{
	return m_device->DrawIndexedPrimitive(PrimitiveType, minIndex, NumVertices, startIndex, primCount);
}

HRESULT WINAPI CDirect3DDevice8::DrawPrimitiveUP(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return m_device->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT WINAPI CDirect3DDevice8::DrawIndexedPrimitiveUP(THIS_ D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertexIndices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride)
{
	return m_device->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertexIndices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);
}

HRESULT WINAPI CDirect3DDevice8::ProcessVertices(THIS_ UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer8* pDestBuffer, DWORD Flags)
{
	return m_device->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, Flags);
}

HRESULT WINAPI CDirect3DDevice8::CreateVertexShader(THIS_ CONST DWORD* pDeclaration, CONST DWORD* pFunction, DWORD* pHandle, DWORD Usage)
{
	return m_device->CreateVertexShader(pDeclaration, pFunction, pHandle, Usage);
}

HRESULT WINAPI CDirect3DDevice8::SetVertexShader(THIS_ DWORD Handle)
{
	return m_device->SetVertexShader(Handle);
}

HRESULT WINAPI CDirect3DDevice8::GetVertexShader(THIS_ DWORD* pHandle)
{
	return m_device->GetVertexShader(pHandle);
}

HRESULT WINAPI CDirect3DDevice8::DeleteVertexShader(THIS_ DWORD Handle)
{
	return m_device->DeleteVertexShader(Handle);
}

HRESULT WINAPI CDirect3DDevice8::SetVertexShaderConstant(THIS_ DWORD Register, CONST void* pConstantData, DWORD ConstantCount)
{
	return m_device->SetVertexShaderConstant(Register, pConstantData, ConstantCount);
}

HRESULT WINAPI CDirect3DDevice8::GetVertexShaderConstant(THIS_ DWORD Register, void* pConstantData, DWORD ConstantCount)
{
	return m_device->GetVertexShaderConstant(Register, pConstantData, ConstantCount);
}

HRESULT WINAPI CDirect3DDevice8::GetVertexShaderDeclaration(THIS_ DWORD Handle, void* pData, DWORD* pSizeOfData)
{
	return m_device->GetVertexShaderDeclaration(Handle, pData, pSizeOfData);
}

HRESULT WINAPI CDirect3DDevice8::GetVertexShaderFunction(THIS_ DWORD Handle, void* pData, DWORD* pSizeOfData)
{
	return m_device->GetVertexShaderFunction(Handle, pData, pSizeOfData);
}

HRESULT WINAPI CDirect3DDevice8::SetStreamSource(THIS_ UINT StreamNumber, IDirect3DVertexBuffer8* pStreamData, UINT Stride)
{
	return m_device->SetStreamSource(StreamNumber, pStreamData, Stride);
}

HRESULT WINAPI CDirect3DDevice8::GetStreamSource(THIS_ UINT StreamNumber, IDirect3DVertexBuffer8** ppStreamData, UINT* pStride)
{
	return m_device->GetStreamSource(StreamNumber, ppStreamData, pStride);
}

HRESULT WINAPI CDirect3DDevice8::SetIndices(THIS_ IDirect3DIndexBuffer8* pIndexData, UINT BaseVertexIndex)
{
	return m_device->SetIndices(pIndexData, BaseVertexIndex);
}

HRESULT WINAPI CDirect3DDevice8::GetIndices(THIS_ IDirect3DIndexBuffer8** ppIndexData, UINT* pBaseVertexIndex)
{
	return m_device->GetIndices(ppIndexData, pBaseVertexIndex);
}

HRESULT WINAPI CDirect3DDevice8::CreatePixelShader(THIS_ CONST DWORD* pFunction, DWORD* pHandle)
{
	return m_device->CreatePixelShader(pFunction, pHandle);
}

HRESULT WINAPI CDirect3DDevice8::SetPixelShader(THIS_ DWORD Handle)
{
	return m_device->SetPixelShader(Handle);
}

HRESULT WINAPI CDirect3DDevice8::GetPixelShader(THIS_ DWORD* pHandle)
{
	return m_device->GetPixelShader(pHandle);
}

HRESULT WINAPI CDirect3DDevice8::DeletePixelShader(THIS_ DWORD Handle)
{
	return m_device->DeletePixelShader(Handle);
}

HRESULT WINAPI CDirect3DDevice8::SetPixelShaderConstant(THIS_ DWORD Register, CONST void* pConstantData, DWORD ConstantCount)
{
	return m_device->SetPixelShaderConstant(Register, pConstantData, ConstantCount);
}
HRESULT WINAPI CDirect3DDevice8::GetPixelShaderConstant(THIS_ DWORD Register, void* pConstantData, DWORD ConstantCount)
{
	return m_device->GetPixelShaderConstant(Register, pConstantData, ConstantCount);
}

HRESULT WINAPI CDirect3DDevice8::GetPixelShaderFunction(THIS_ DWORD Handle, void* pData, DWORD* pSizeOfData)
{
	return m_device->GetPixelShaderFunction(Handle, pData, pSizeOfData);
}

HRESULT WINAPI CDirect3DDevice8::DrawRectPatch(THIS_ UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo)
{
	return m_device->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo);
}

HRESULT WINAPI CDirect3DDevice8::DrawTriPatch(THIS_ UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo)
{
	return m_device->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo);
}

HRESULT WINAPI CDirect3DDevice8::DeletePatch(THIS_ UINT Handle)
{
	return m_device->DeletePatch(Handle);
}
