#pragma once

#include "pch.h"
#include "Direct3D.h"  

// Base class for D3D11-rendered view. Owns render target and it's textures, also owns the backbuffer texture for WPF interop.
class ViewBase
{
public:
    ViewBase();
    virtual ~ViewBase();

protected:
    // Creates or re-creates the render target and shared surface for a specific size.
    HRESULT createResources(UINT width, UINT height);
    void releaseResources();

    // --- Protected Member Variables ---
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    Microsoft::WRL::ComPtr<IDirect3DSurface9> m_sharedSurface;
    HANDLE m_sharedTextureHandle = nullptr;

    // cached size of the current render target
    UINT m_width = 0;
    UINT m_height = 0;
};