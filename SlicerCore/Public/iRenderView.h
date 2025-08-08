#pragma once

#include <atltypes.h>
#include <Unknwn.h>

__interface __declspec(uuid("17941d20-efb9-494b-b8d7-a23b97b76d4e")) iRenderView : public IUnknown
{
	HRESULT __stdcall render();

	HRESULT __stdcall resize(const CSize& sizePixels, float dpiScale);

	// WPF D3DImage only support DirectX 9, But we use DirectX 11 for rendering. We will pass the IDirect3DSurface9 to WPF to render from here,
	// WPF thinks it is a DirectX 9 surface but it was actually rendered by DirectX 11.
	HRESULT __stdcall getSurface(void** ppSurface);

	// outputs if the ppsurface is available for rendering, if tried to render when the surface not available, it may crash.
	HRESULT __stdcall frontBufferAvailableChanged(BOOL nowAvailable);	

};
