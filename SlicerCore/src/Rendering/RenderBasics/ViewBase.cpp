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
    m_rasterizerState.Reset();
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

    // depth buffer
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTex;
    HRESULT hr = device11->CreateTexture2D(&depthDesc, nullptr, &depthTex);
    if (FAILED(hr))
        return hr;

    hr = device11->CreateDepthStencilView(depthTex.Get(), nullptr, &m_depthStencilView);
    if (FAILED(hr))
        return hr;

    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.DepthClipEnable = TRUE;
    hr = device11->CreateRasterizerState(&rsDesc, &m_rasterizerState);
    if (FAILED(hr))
        return hr;
    context->RSSetState(m_rasterizerState.Get());

    // Create the RTV
    hr = device11->CreateRenderTargetView(d3d11Tex.Get(), nullptr, &m_renderTargetView);
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

    XMVECTOR vmin = XMLoadFloat3(&sMin);
    XMVECTOR vmax = XMLoadFloat3(&sMax);
    XMVECTOR centerV = (vmin + vmax) * 0.5f;        

    float cx = XMVectorGetX(centerV);
    float cy = XMVectorGetY(centerV);
    float cz = XMVectorGetZ(centerV);

    XMVECTOR diagV = vmax - vmin;                      // bbox diagonal
    float dx = XMVectorGetX(diagV);
    float dy = XMVectorGetY(diagV);
    float dz = XMVectorGetZ(diagV);
    float R = 0.5f * std::sqrt(dx * dx + dy * dy + dz * dz); // bounding-sphere radius
    R = max(R, 1e-4f);

    XMMATRIX M_center = XMMatrixTranslation(-XMVectorGetX(centerV), -XMVectorGetY(centerV), -XMVectorGetZ(centerV));
    XMMATRIX M_rot = XMMatrixRotationRollPitchYaw(rs.yaw, rs.pitch, 0.0f);
    XMMATRIX M_pan = XMMatrixTranslation(-rs.pan.x - cx, -rs.pan.y - cy, -rs.pan.z - cz);
    XMMATRIX M = M_pan * M_rot * M_center;   

    // VIEW 
    float d = 4 * R - rs.distance;

    XMVECTOR dir = XMLoadFloat3(&rs.viewDir);
    dir = XMVector3Normalize(dir);
    XMVECTOR eye = XMVectorScale(dir, d);
    XMVECTOR at = XMVectorSet(-cx, -cy, -cz, 0);
    XMVECTOR up = XMVectorSet(0, 0, 1, 0);
    XMMATRIX V = XMMatrixLookAtLH(eye, at, up);

    // PROJECTION 
    float aspect = (rs.height > 0) ? float(rs.width) / float(rs.height) : 1.0f;
    XMMATRIX P;

    if (rs.projection == ProjectionMode::Perspective)
    {
        float fovY = (rs.fovY > 0.f) ? rs.fovY : XM_PIDIV4;
        P = XMMatrixPerspectiveFovLH(fovY, aspect, 0.01f, 1000.0f);
    }
    else
    {
        float halfH = max(rs.distance, 0.01f);
        float halfW = halfH * aspect;
        P = XMMatrixOrthographicLH(2.f * halfW, 2.f * halfH, 0.01f, 1000.0f);
    }

    // upload MVP
    mvpCB cb{};
    XMMATRIX MVP = M * V * P;
    DirectX::XMStoreFloat4x4(&cb.MVP, XMMatrixTranspose(MVP));

    D3D11_MAPPED_SUBRESOURCE mapped{};
    Direct3D::context->Map(m_cbMVP.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &cb, sizeof(cb));
    Direct3D::context->Unmap(m_cbMVP.Get(), 0);

    ID3D11Buffer* cbuffers[] = { m_cbMVP.Get() };
    Direct3D::context->VSSetConstantBuffers(0, 1, cbuffers);

}

void ViewBase::updateViewMode(int mode, RenderState& renderState)
{
    ViewMode vm = static_cast<ViewMode>(mode);
    switch (vm)
    {
    case ViewMode::Front:  renderState.viewDir = { 1.f, 0.f, 0.f }; break;
    case ViewMode::Back:   renderState.viewDir = { -1.f, 0.f, 0.f }; break;
    case ViewMode::Left:   renderState.viewDir = { 0.f, 1.f, 0.f }; break;
    case ViewMode::Right:  renderState.viewDir = { 0.f,-1.f, 0.f }; break;
    case ViewMode::Top:    renderState.viewDir = { 0.f, 0.f,-1.f }; break;
    case ViewMode::Bottom: renderState.viewDir = { 0.f, 0.f, 1.f }; break;
    default: break;
    }
    renderState.yaw = 0.0f;
    renderState.pitch = 0.0f;
}

