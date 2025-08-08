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

    // --- Create the Shared DX9 Surface ---
    hr = Direct3D::device9->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET,
        D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_sharedSurface, &m_sharedTextureHandle);

    // (Depth buffer creation would go here as well)

    return hr;
}