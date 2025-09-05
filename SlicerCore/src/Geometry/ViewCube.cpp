#pragma once
#include "ViewCube.h"
#include "../Rendering/RenderBasics/Direct3D.h"

using namespace Direct3D;
using namespace DirectX;

struct CubeVertex
{
    XMFLOAT3 pos;
    XMFLOAT3 normal;
};

HRESULT createViewCube(HWND hWnd, iViewCube** ppView)
{
    HRESULT hr = Direct3D::createDevices(hWnd);
    if (FAILED(hr))
        return hr;

    CComPtr<CComObject<ViewCube>> view;
    hr = createInstance(view);
    if (FAILED(hr))
        return hr;

    *ppView = view.Detach();
    return S_OK;
}

HRESULT ViewCube::initializeCube()
{
    if (initialized)
        return S_OK;

    // Cube geometry (unit cube)
    CubeVertex vertices[24] =
    {
        // +X
        { {1, 1, 1}, {1, 0, 0} },
        { {1, 1,-1}, {1, 0, 0} },
        { {1,-1,-1}, {1, 0, 0} },
        { {1,-1, 1}, {1, 0, 0} },
        // -X
        { {-1, 1,-1}, {-1, 0, 0} },
        { {-1, 1, 1}, {-1, 0, 0} },
        { {-1,-1, 1}, {-1, 0, 0} },
        { {-1,-1,-1}, {-1, 0, 0} },
        // +Y
        { {-1, 1,-1}, {0, 1, 0} },
        { { 1, 1,-1}, {0, 1, 0} },
        { { 1, 1, 1}, {0, 1, 0} },
        { {-1, 1, 1}, {0, 1, 0} },
        // -Y
        { {-1,-1, 1}, {0,-1, 0} },
        { { 1,-1, 1}, {0,-1, 0} },
        { { 1,-1,-1}, {0,-1, 0} },
        { {-1,-1,-1}, {0,-1, 0} },
        // +Z
        { { 1, 1, 1}, {0, 0, 1} },
        { {-1, 1, 1}, {0, 0, 1} },
        { {-1,-1, 1}, {0, 0, 1} },
        { { 1,-1, 1}, {0, 0, 1} },
        // -Z
        { {-1, 1,-1}, {0, 0,-1} },
        { { 1, 1,-1}, {0, 0,-1} },
        { { 1,-1,-1}, {0, 0,-1} },
        { {-1,-1,-1}, {0, 0,-1} },
    };

    uint16_t indices[36] =
    {
        0,1,2, 0,2,3,       // +X
        4,5,6, 4,6,7,       // -X
        8,9,10, 8,10,11,    // +Y
        12,13,14, 12,14,15, // -Y
        16,17,18, 16,18,19, // +Z
        20,21,22, 20,22,23  // -Z
    };

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = sizeof(vertices);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vinit = { vertices };
    HRESULT hr = device11->CreateBuffer(&vbd, &vinit, &m_vertexBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(indices);
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA iinit = { indices };
    hr = device11->CreateBuffer(&ibd, &iinit, &m_indexBuffer);
    if (FAILED(hr))
        return hr;

    initialized = true;
    return S_OK;
}

HRESULT __stdcall ViewCube::render()
{
    if (!initialized)
        initializeCube();

    // Save current render targets and viewport to avoid affecting other views
    CComPtr<ID3D11RenderTargetView> oldRTV;
    CComPtr<ID3D11DepthStencilView> oldDSV;
    Direct3D::context->OMGetRenderTargets(1, &oldRTV, &oldDSV);

    UINT num = 1;
    D3D11_VIEWPORT oldVp;
    Direct3D::context->RSGetViewports(&num, &oldVp);

    float clearColor[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    if (Direct3D::context && m_renderTargetView)
    {
        ID3D11RenderTargetView* rtv = m_renderTargetView.Get();
        Direct3D::context->OMSetRenderTargets(1, &rtv, m_depthStencilView.Get());
        Direct3D::context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
        if (m_depthStencilView)
            Direct3D::context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        Direct3D::context->RSSetState(m_rasterizerState.Get());
        D3D11_VIEWPORT vp = { 0.0f, 0.0f, renderState.width, renderState.height, 0.0f, 1.0f };
        Direct3D::context->RSSetViewports(1, &vp);
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
    };

    HRESULT hr = Direct3D::BindShadersFromCSO(L"SimpleVS.cso", L"ViewCubePS.cso", layout, _countof(layout), &m_vs, &m_ps, &m_il);
    if (FAILED(hr))
        return hr;

    UINT stride = sizeof(CubeVertex);
    UINT offset = 0;
    ID3D11Buffer* vb = m_vertexBuffer.Get();
    Direct3D::context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    Direct3D::context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    Direct3D::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    Direct3D::context->IASetInputLayout(m_il.Get());
    Direct3D::context->VSSetShader(m_vs.Get(), nullptr, 0);
    Direct3D::context->PSSetShader(m_ps.Get(), nullptr, 0);

    Direct3D::context->DrawIndexed(36, 0, 0);
    Direct3D::context->Flush();

    // Restore previous render targets and viewport
    ID3D11RenderTargetView* rtvRestore = oldRTV.p;
    Direct3D::context->OMSetRenderTargets(1, &rtvRestore, oldDSV.p);
    Direct3D::context->RSSetViewports(1, &oldVp);

    return S_OK;
}

HRESULT __stdcall ViewCube::resize(const int widthPixels, const int heightPixels, const float dpiScale)
{
    renderState.width = (float)widthPixels;
    renderState.height = (float)heightPixels;
    renderState.dpi = dpiScale;
    return createResources(widthPixels, heightPixels);
}

HRESULT __stdcall ViewCube::getSurface(void** ppSurface)
{
    if (!ppSurface)
        return E_POINTER;
    *ppSurface = m_sharedSurface.Get();
    if (m_sharedSurface)
        m_sharedSurface->AddRef();
    return S_OK;
}

HRESULT __stdcall ViewCube::rotate(float dx, float dy)
{
    renderState.yaw += dx * 0.01f;
    renderState.pitch += dy * 0.01f;
    return S_OK;
}

HRESULT __stdcall ViewCube::pick(int x, int y, int* faceId)
{
    return E_NOTIMPL;
}

HRESULT __stdcall ViewCube::setHighlight(int faceId)
{
    return E_NOTIMPL;
}