#pragma once
#include "MainView.h"
#include "../pch.h"
#include "../../src/Rendering/RenderBasics/Direct3D.h"
#include "../Geometry/GeometryManager.h"


//#define RENDERDOC_NOHELPER 
//#include "renderdoc_app.h"
//static RENDERDOC_API_1_6_0* g_rdoc = nullptr;
//void InitRenderDocAPI();

using namespace Direct3D;
using namespace DirectX;



HRESULT createMainView(HWND hWnd, iMainView** ppView)
{
    HRESULT hr = Direct3D::createDevices(hWnd);
    if (FAILED(hr))
        return hr;

    CComPtr<CComObject<MainView>> mainView;
    hr = createInstance(mainView);
    if (FAILED(hr))
        return hr;

    *ppView = mainView.Detach();
    return S_OK;
}


HRESULT __stdcall MainView::render()
{
    /*if (!renderdocLoaded)
    {
        InitRenderDocAPI();
        renderdocLoaded = true;
    }

    if (g_rdoc) g_rdoc->StartFrameCapture(device11, nullptr);*/


    // setting the background
    float clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    if (Direct3D::context && m_renderTargetView)
    {
        ID3D11RenderTargetView* rtv = m_renderTargetView.Get();
        Direct3D::context->OMSetRenderTargets(1, &rtv, nullptr);
        ID3D11DepthStencilView* dsv = m_depthStencilView.Get();
        Direct3D::context->OMSetRenderTargets(1, &rtv, dsv);
        Direct3D::context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
        if (dsv)
            Direct3D::context->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        Direct3D::context->RSSetState(m_rasterizerState.Get());
    }

    // rendering the geometries
    if (geometryManager == NULL || GeomCount() <= 0)
    {
        Direct3D::context->Flush();
        return S_OK;
    }

    // update model view proj
    BoundingBox bb;
    geometryManager->GetGlobalBoundingBox(bb);

    if (bbox != bb)
    {
        grid.Initialize(bb);
        bbox = bb;
    }

    UpdateMVPCBuffer(bb, renderState);

    // 1 - render bottom grid
    grid.Render();

    // 2 - render geometries
    geometryManager->RenderGeometries();


    Direct3D::context->Flush();

    //if (g_rdoc) g_rdoc->EndFrameCapture(Direct3D::device11, nullptr);

    return S_OK;
}

HRESULT __stdcall MainView::resize(const int widthPixels, const int heightPixels, const float dpiScale)
{
    renderState.width  = (float)widthPixels;
    renderState.height = (float)heightPixels;
    renderState.dpi = dpiScale;
    return createResources(widthPixels, heightPixels);
}

HRESULT __stdcall MainView::getSurface(void** ppSurface)
{
    if (!ppSurface) 
        return E_POINTER;
    *ppSurface = m_sharedSurface.Get();
    if (m_sharedSurface)
        m_sharedSurface->AddRef();
    return S_OK;
}

HRESULT __stdcall MainView::setGeometryManager(iGeometryManager* geomManger)
{
    geometryManager = geomManger;
    resetView();
    return S_OK;
}

HRESULT __stdcall MainView::zoom(float delta)
{
    renderState.distance *= (1.0f - delta * 0.002f);
    if (renderState.distance < 0.01f) 
        renderState.distance = 0.01f;
    return S_OK;
}

HRESULT __stdcall MainView::rotate(float dx, float dy)
{
    renderState.yaw += dx * 0.005f;
    renderState.pitch += dy * 0.005f;
    const float limit = DirectX::XM_PIDIV2 - 0.01f;
    if (renderState.pitch >  limit)
        renderState.pitch = limit;
    if (renderState.pitch < -limit)
        renderState.pitch = -limit;
    return S_OK;
}

HRESULT __stdcall MainView::pan(float dx, float dy)
{
    XMMATRIX rot = XMMatrixRotationRollPitchYaw(renderState.pitch, renderState.yaw, 0.0f);
    XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rot);
    XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rot);
    XMVECTOR move = XMVectorScale(right, -dx * 0.002f) + XMVectorScale(up, dy * 0.002f);
    XMVECTOR pan = XMLoadFloat3(&renderState.pan);
    pan += move;
    XMStoreFloat3(&renderState.pan, pan);
    return S_OK;
}

int MainView::GeomCount()
{
    int geomCount = 0;
    geometryManager->getGeometryCount(geomCount);
    return geomCount;
}

HRESULT __stdcall MainView::resetView()
{
    renderState.yaw = 0.0f;
    renderState.pitch = 0.0f; 
    renderState.pan = { 0.f, 0.f, 0.f };
    renderState.distance = (renderState.projection == ProjectionMode::Perspective) ? 2.0f : 1.0f;
    return S_OK;
}

HRESULT __stdcall MainView::setProjection(int mode)
{
    renderState.projection = (mode == 0) ? ProjectionMode::Perspective : ProjectionMode::Orthographic;
    renderState.distance = (renderState.projection == ProjectionMode::Perspective) ? 2.0f : 1.0f;
    return S_OK;
}

HRESULT __stdcall MainView::setViewMode(int mode)
{
    updateViewMode(mode, renderState);
    return S_OK;
}

HRESULT __stdcall MainView::pickGeom(int x, int y, GUID& geomId)
{
    // update model view proj
    BoundingBox bb;
    geometryManager->GetGlobalBoundingBox(bb);
    UpdateMVPCBuffer(bb, renderState);

    geometryManager->PickGeometry(x, y, geomId, renderState);
    return S_OK;
}

HRESULT __stdcall MainView::pickGeomArrow(int x, int y, GUID geomId, eViewDirection& viewDir)
{
    BoundingBox bb;
    geometryManager->GetGlobalBoundingBox(bb);

    geometryManager->PickGeomArrow(x, y, geomId, bb, renderState, viewDir);
    return S_OK;
}



//void InitRenderDocAPI()
//{
//    HMODULE mod = GetModuleHandleA("renderdoc.dll");
//    if (!mod)
//    {
//        mod = LoadLibraryA("renderdoc.dll");
//    }
//
//    if (mod)
//    {
//        pRENDERDOC_GetAPI RENDERDOC_GetAPI = (pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
//        if (RENDERDOC_GetAPI)
//            RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_6_0, (void**)&g_rdoc);
//    }
//}
//
