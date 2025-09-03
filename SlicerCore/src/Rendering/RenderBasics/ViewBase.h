#pragma once
#include "Direct3D.h"  

// Base class for D3D11-rendered view. Owns render target and it's textures, also owns the backbuffer texture for WPF interop.
class ViewBase
{
public:
    ViewBase();
    virtual ~ViewBase();

    void UpdateMVPCBuffer(BoundingBox globalBB, RenderState rs);

protected:
    // get called on every resizing
    HRESULT createResources(UINT width, UINT height);
    void createMVPCBuffer();
    void releaseResources();

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    Microsoft::WRL::ComPtr<IDirect3DSurface9> m_sharedSurface;
    HANDLE m_sharedTextureHandle = nullptr;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_cbMVP;  // pass mvp matrix to shaders
};