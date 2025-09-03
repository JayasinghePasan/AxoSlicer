#pragma once
#include "../../pch.h"
#include "ViewBase.h"
#include "DirectXMath.h"

using namespace Direct3D;
using namespace DirectX;

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

    // create viewport
    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0.0f;  vp.TopLeftY = 0.0f;
    vp.Width = static_cast<float>(width);
    vp.Height = static_cast<float>(height);
    vp.MinDepth = 0.0f;  vp.MaxDepth = 1.0f;

    Direct3D::context->RSSetViewports(1, &vp);

    // Create the RTV
    HRESULT hr = device11->CreateRenderTargetView(d3d11Tex.Get(), nullptr, &m_renderTargetView);
    if (FAILED(hr)) 
        return hr;

    createMVPCBuffer();

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
        &sharedHandle);

    d3d9Tex->GetSurfaceLevel(0, &m_sharedSurface);

    if (FAILED(hr))
       return hr;

    return hr;
}

void ViewBase::createMVPCBuffer()
{
    D3D11_BUFFER_DESC cbd = {};
    cbd.ByteWidth = sizeof(mvpCB);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    Direct3D::device11->CreateBuffer(&cbd, nullptr, m_cbMVP.GetAddressOf());
}

void ViewBase::UpdateMVPCBuffer(BoundingBox globalBB, RenderState rs)
{

    XMFLOAT3 sMin(globalBB.minX, globalBB.minY, globalBB.minZ);
    XMFLOAT3 sMax(globalBB.maxX, globalBB.maxY, globalBB.maxZ);

    XMVECTOR minV = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&sMin));
    XMVECTOR maxV = XMLoadFloat3(reinterpret_cast<XMFLOAT3*>(&sMax));
    XMVECTOR center = (minV + maxV) * 0.5f;
    XMVECTOR extent = XMVectorMax(maxV - minV, XMVectorReplicate(1e-6f));

    float maxExtent = *std::max_element(
        std::begin(std::initializer_list<float>{ XMVectorGetX(extent), XMVectorGetY(extent), XMVectorGetZ(extent) }),
        std::end(std::initializer_list<float>{ XMVectorGetX(extent), XMVectorGetY(extent), XMVectorGetZ(extent) })
    );

    // Normalize scene to ~[-1,1] to make the camera easy
    XMMATRIX scene = XMMatrixScaling(2.f / maxExtent, 2.f / maxExtent, 2.f / maxExtent) * XMMatrixTranslation(-XMVectorGetX(center), -XMVectorGetY(center), -XMVectorGetZ(center));

    // Camera from your yaw/pitch/distance/pan
    XMMATRIX rot = XMMatrixRotationRollPitchYaw(rs.pitch, rs.yaw, 0.0f);
    XMVECTOR eye = XMVector3TransformCoord(XMVectorSet(0, 0, -rs.distance, 1), rot) + XMVectorSet(rs.pan.x, rs.pan.y, 0, 0);
    XMVECTOR at = XMVectorSet(rs.pan.x, rs.pan.y, 0, 0);
    XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rot);
    XMMATRIX view = XMMatrixLookAtLH(eye, at, up);

    // IMPORTANT: fix integer division here
    float aspect = (rs.height > 0) ? (float)rs.width / (float)rs.height : 1.0f;
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.01f, 100.0f);

    // If HLSL is row_major, do NOT transpose; otherwise transpose here:
    mvpCB cb{};
    XMStoreFloat4x4(&cb.MVP, XMMatrixTranspose(scene * view * proj));

    // Update CB
    D3D11_MAPPED_SUBRESOURCE mapped{};
    Direct3D::context->Map(m_cbMVP.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &cb, sizeof(cb));
    Direct3D::context->Unmap(m_cbMVP.Get(), 0);

    // Bind to VS slot b0
    ID3D11Buffer* cbuffers[] = { m_cbMVP.Get() };
    Direct3D::context->VSSetConstantBuffers(0, 1, cbuffers);
}
