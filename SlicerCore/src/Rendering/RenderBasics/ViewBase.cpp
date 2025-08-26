#include "pch.h"
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
    if (m_sharedTextureHandle)
    {
        CloseHandle(m_sharedTextureHandle);
        m_sharedTextureHandle = nullptr;
    }
}

HRESULT ViewBase::createResources(UINT width, UINT height)
{
    // First, release any old resources.
    releaseResources();

    m_width = width;
    m_height = height;

    // Use the global Direct3D::device to create our resources.
    if (!Direct3D::device || !Direct3D::device9)
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
    desc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX; // For sharing with DX9

    Microsoft::WRL::ComPtr<ID3D11Texture2D> d3d11Texture;
    HRESULT hr = Direct3D::device->CreateTexture2D(&desc, nullptr, &d3d11Texture);
    if (FAILED(hr)) return hr;

    // --- Create the Render Target View for the DX11 Texture ---
    hr = Direct3D::device->CreateRenderTargetView(d3d11Texture.Get(), nullptr, &m_renderTargetView);
    if (FAILED(hr)) return hr;

    // --- Get the Handle for Sharing ---
    Microsoft::WRL::ComPtr<IDXGIResource1> dxgiResource;
    hr = d3d11Texture.As(&dxgiResource);
    if (FAILED(hr)) return hr;
    hr = dxgiResource->GetSharedHandle(&m_sharedTextureHandle);
    if (FAILED(hr)) return hr;

    // --- Create Depth/Stencil ---
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
    hr = Direct3D::device->CreateTexture2D(&depthDesc, nullptr, &depthTexture);
    if (FAILED(hr)) return hr;

    hr = Direct3D::device->CreateDepthStencilView(depthTexture.Get(), nullptr, &m_depthStencilView);
    if (FAILED(hr)) return hr;

    // --- Create the Shared DX9 Surface ---
    Microsoft::WRL::ComPtr<IDirect3DTexture9> d3d9Texture;
    hr = Direct3D::device9->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &d3d9Texture, &m_sharedTextureHandle);
    if (FAILED(hr)) return hr;

    hr = d3d9Texture->GetSurfaceLevel(0, &m_sharedSurface);
    return hr;
}