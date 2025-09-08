#pragma once
#include "ViewCube.h"
#include "../Rendering/RenderBasics/Direct3D.h"

#define RENDERDOC_NOHELPER 
#include "../Rendering/renderdoc_app.h"
static RENDERDOC_API_1_6_0* g_rdoc = nullptr;

void InitRenderDocAPI()
{
    HMODULE mod = GetModuleHandleA("renderdoc.dll");
    if (!mod)
    {
        mod = LoadLibraryA("renderdoc.dll");
    }

    if (mod)
    {
        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
        if (RENDERDOC_GetAPI)
            RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_6_0, (void**)&g_rdoc);
    }
}



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
        0,2,1, 0,3,2,       // +X
        4,6,5, 4,7,6,       // -X
        8,10,9, 8,11,10,    // +Y
        12,14,13, 12,15,14, // -Y
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

    createMVPCBuffer();

    // constant buffer for hover highlighting
    D3D11_BUFFER_DESC cbd = {};
    cbd.ByteWidth = sizeof(int) * 4; // 16 bytes alignment
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = device11->CreateBuffer(&cbd, nullptr, &m_highlightCB);
    if (FAILED(hr))
        return hr;

    setHighlight(0); // initialize to no highlight

    initialized = true;
    return S_OK;
}

HRESULT ViewCube::initializePick()
{
    if (initializedPick)
        return S_OK;
    
    HRESULT hr;

    // render texture
    CD3D11_TEXTURE2D_DESC TxDesc{ DXGI_FORMAT_R32_UINT, 1,1,1,1, D3D11_BIND_RENDER_TARGET };
    hr = device11->CreateTexture2D(&TxDesc, nullptr, &m_pickTexture);
    if (FAILED(hr))
        return hr;

    // render target
    CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc{ D3D11_RTV_DIMENSION_TEXTURE2D, DXGI_FORMAT_R32_UINT };
    hr = device11->CreateRenderTargetView(m_pickTexture.Get(), &rtvDesc, &m_pickRTV);
    if (FAILED(hr))
        return hr;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTexture;
    CD3D11_TEXTURE2D_DESC txDepthDesc { DXGI_FORMAT_D32_FLOAT, 1,1,1,1, D3D11_BIND_DEPTH_STENCIL };
    hr = device11->CreateTexture2D(&txDepthDesc, nullptr, &depthTexture);
    if (FAILED(hr))
        return hr;

    CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{ D3D11_DSV_DIMENSION_TEXTURE2D, DXGI_FORMAT_D32_FLOAT };
    device11->CreateDepthStencilView(depthTexture.Get(), &dsvDesc, &m_pickDepthStencil);
    if (FAILED(hr))
        return hr;

    // staging texture
    CD3D11_TEXTURE2D_DESC stageTxDesc{ DXGI_FORMAT_R32_UINT, 1,1,1,1,0,D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ};
    hr = device11->CreateTexture2D(&stageTxDesc, nullptr, &m_pickTextureStaging);

    initializedPick = true;

    return hr;
}

HRESULT __stdcall ViewCube::render()
{
    if (!initialized)
        initializeCube();

    BoundingBox bb(-1.f, -1.f, -1.f, 1.f, 1.f, 1.f);
    UpdateMVPCBuffer(bb, renderState);

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

    // bind highlight constant buffer (slot 1)
    ID3D11Buffer* hcb = m_highlightCB.Get();
    Direct3D::context->PSSetConstantBuffers(1, 1, &hcb);

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
    renderState.distance = 2.0f;
    
    HRESULT hr = createResources(widthPixels, heightPixels);
    if (SUCCEEDED(hr))
    {
        D3D11_RASTERIZER_DESC rsDesc = {};
        rsDesc.FillMode = D3D11_FILL_SOLID;
        rsDesc.CullMode = D3D11_CULL_NONE;
        rsDesc.DepthClipEnable = FALSE;
        hr = Direct3D::device11->CreateRasterizerState(&rsDesc, &m_rasterizerState);
    }
    return hr;
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
    renderState.yaw += dx * 0.005f;
    renderState.pitch += dy * 0.005f;
    const float limit = DirectX::XM_PIDIV2 - 0.01f;
    if (renderState.pitch > limit)
        renderState.pitch = limit;
    if (renderState.pitch < -limit)
        renderState.pitch = -limit;
    return S_OK;
}

HRESULT __stdcall ViewCube::pick(int x, int y, int* faceId)
{
    /*if (!renderdocLoaded)
    {
        InitRenderDocAPI();
        renderdocLoaded = true;
    }

    if (g_rdoc) g_rdoc->StartFrameCapture(device11, nullptr);*/

    if (!initializedPick)
        initializePick();

    // Save current render targets and viewport so picking does not interfere with other views using the same device context
    CComPtr<ID3D11RenderTargetView> oldRTV;
    CComPtr<ID3D11DepthStencilView> oldDSV;
    context->OMGetRenderTargets(1, &oldRTV, &oldDSV);
    UINT num = 1;
    D3D11_VIEWPORT oldVP;
    context->RSGetViewports(&num, &oldVP);

    CComPtr<ID3D11BlendState> oldBlendState;
    FLOAT blendFactor[4];
    UINT sampleMask;
    context->OMGetBlendState(&oldBlendState, blendFactor, &sampleMask);
    context->OMSetBlendState(nullptr, nullptr, 0xffffffff);

    // clear  rtv
    float clearCol[4] = {};
    context->ClearRenderTargetView(m_pickRTV.Get(), clearCol);
    context->ClearDepthStencilView(m_pickDepthStencil.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
    context->OMSetRenderTargets(1, m_pickRTV.GetAddressOf(), m_pickDepthStencil.Get());

    // creating a 1 pixel viewport at (x,y)
    D3D11_VIEWPORT pixelViewPort;
    pixelViewPort.Width = renderState.width;
    pixelViewPort.Height = renderState.height;
    pixelViewPort.TopLeftX = (float)-x;
    pixelViewPort.TopLeftY = (float)-y;
    pixelViewPort.MinDepth = 0;
    pixelViewPort.MaxDepth = 1;
    context->RSSetViewports(1, &pixelViewPort);

    BoundingBox bb(-1.f, -1.f, -1.f, 1.f, 1.f, 1.f);
    UpdateMVPCBuffer(bb, renderState);

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0, 0,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"NORMAL"  ,0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0}
    };

    HRESULT hr = Direct3D::BindShadersFromCSO(L"SimpleVS.cso", L"ViewCubePickPS.cso", layout, _countof(layout), nullptr, nullptr, nullptr);
    if (FAILED(hr))
        return hr;

    // render the cube
    UINT stride = sizeof(CubeVertex);
    UINT offset = 0;
    ID3D11Buffer* vb = m_vertexBuffer.Get();
    Direct3D::context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    Direct3D::context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    Direct3D::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Direct3D::context->DrawIndexed(36, 0, 0);

    ID3D11RenderTargetView* rtv = nullptr;
    context->OMSetRenderTargets(1, &rtv, nullptr);
    context->CopyResource(m_pickTextureStaging.Get(), m_pickTexture.Get());

    D3D11_MAPPED_SUBRESOURCE map;
    hr = context->Map(m_pickTextureStaging.Get(), 0, D3D11_MAP_READ, 0, &map);
    if (FAILED(hr))
        return hr;

    const uint32_t* const primID = (uint32_t*)map.pData;
    int triangleId = static_cast<int>(*primID) - 1; 
    *faceId = triangleId >= 0 ? triangleId / 2 : -1;
    context->Unmap(m_pickTextureStaging.Get(), 0);

    //if (g_rdoc) g_rdoc->EndFrameCapture(Direct3D::device11, nullptr);
    
    // Restore previous render targets and viewport
    ID3D11RenderTargetView* rtvRestore = oldRTV.p;
    context->OMSetRenderTargets(1, &rtvRestore, oldDSV.p);
    context->RSSetViewports(1, &oldVP);
    context->OMSetBlendState(oldBlendState.p, blendFactor, sampleMask);

    return S_OK;
}

HRESULT __stdcall ViewCube::setHighlight(unsigned int faceMask)
{
    m_highlightMask = faceMask;
    if (!m_highlightCB)
        return S_OK;

    struct 
    { 
        unsigned int mask; 
        unsigned int padding[3]; 
    } 
    cbData = { faceMask,{0,0,0} };
    D3D11_MAPPED_SUBRESOURCE mapped{};
    Direct3D::context->Map(m_highlightCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &cbData, sizeof(cbData));
    Direct3D::context->Unmap(m_highlightCB.Get(), 0);
    return S_OK;
}

HRESULT __stdcall ViewCube::resetView()
{
    renderState.yaw = 0.0f;
    renderState.pitch = 0.0f;
    return S_OK;
}