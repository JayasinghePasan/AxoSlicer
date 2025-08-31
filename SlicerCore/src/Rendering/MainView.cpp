#include "RenderBasics/pch.h"
#include "MainView.h"
#include "../../Public/SlicerCoreAPI.h" 
#include "../../src/Rendering/RenderBasics/Direct3D.h"

using namespace Direct3D;

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
    float clearColor[] = { 0.8f, 0.9f, 0.9f, 1.0f };
    if (Direct3D::context && m_renderTargetView)
    {
        ID3D11RenderTargetView* rtv = m_renderTargetView.Get();
        Direct3D::context->OMSetRenderTargets(1, &rtv, nullptr);
        Direct3D::context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
    }

    Direct3D::context->Flush();
    return S_OK;
}

HRESULT __stdcall MainView::resize(const int widthPixels, const int heightPixels, const float dpiScale)
{
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

HRESULT __stdcall MainView::frontBufferAvailableChanged(BOOL nowAvailable)
{
    return S_OK;
}