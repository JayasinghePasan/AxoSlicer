#pragma once
#include "../../pch.h"
#include "ViewBase.h"

using namespace Direct3D;

ViewBase::ViewBase() = default;
ViewBase::~ViewBase()
{
    releaseResources();
}

void ViewBase::releaseResources()
{
    m_renderTargetView.Reset();
    m_depthStencilView.Reset();
    m_sharedSurface.Reset();
    m_sharedTextureHandle = nullptr;
}

HRESULT ViewBase::createResources(UINT width, UINT height)
{
    releaseResources();

    if (!Direct3D::device11 || !Direct3D::device9)
        return E_FAIL;

    // --- Create the Shared DX11 Texture ---
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

    // Create shared D3D11 texture
    desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> d3d11Tex;
    device11->CreateTexture2D(&desc, nullptr, &d3d11Tex);

    // Create the RTV
    HRESULT hr = device11->CreateRenderTargetView(d3d11Tex.Get(), nullptr, &m_renderTargetView);
    if (FAILED(hr)) 
        return hr;

    // Get shared handle
    Microsoft::WRL::ComPtr<IDXGIResource> dxgiRes;
    d3d11Tex.As(&dxgiRes);
    HANDLE sharedHandle = nullptr;
    dxgiRes->GetSharedHandle(&sharedHandle);

    // Open that handle in D3D9
    Microsoft::WRL::ComPtr<IDirect3DTexture9> d3d9Tex;
    hr = device9->CreateTexture(
        width, height, 1,
        D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8,
        D3DPOOL_DEFAULT,
        &d3d9Tex,
        &sharedHandle);  // <- actually opens the shared tex

    d3d9Tex->GetSurfaceLevel(0, &m_sharedSurface);

    if (FAILED(hr))
       return hr;

    return hr;
}