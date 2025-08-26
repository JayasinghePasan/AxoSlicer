#include "pch.h"
#include "MainView.h"
#include "../../Public/SlicerCoreAPI.h"
#include "../../src/Rendering/RenderBasics/Direct3D.h"

using namespace Direct3D;

HRESULT createMainView(HWND hWnd, iMainView** ppView)
{
    HRESULT hr = Direct3D::createDevice(hWnd);
    if (FAILED(hr))
        return hr;

    CComPtr<CComObject<MainView>> mainView;
    hr = createInstance(mainView);
    if (FAILED(hr))
        return hr;

    *ppView = mainView.Detach();
    return S_OK;
}


// --- MainView Method Implementations ---

HRESULT __stdcall MainView::render()
{
    // For now simply clear the render target.
    float clearColor[] = { 0.1f, 0.2f, 0.3f, 1.0f };

    if (Direct3D::context && m_renderTargetView)
    {
        CComCritSecLock<CComAutoCriticalSection> lock(g_contextLock);

        ID3D11RenderTargetView* rtv = m_renderTargetView.Get();
        Direct3D::context->OMSetRenderTargets(1, &rtv, m_depthStencilView.Get());

        D3D11_VIEWPORT vp = {};
        vp.Width = static_cast<float>(m_width);
        vp.Height = static_cast<float>(m_height);
        vp.MaxDepth = 1.0f;
        Direct3D::context->RSSetViewports(1, &vp);

        Direct3D::context->ClearRenderTargetView(rtv, clearColor);
        if (m_depthStencilView)
            Direct3D::context->ClearDepthStencilView(m_depthStencilView.Get(),
                D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        Direct3D::context->Flush();
    }

    return S_OK;
}

HRESULT __stdcall MainView::resize(const CSize& sizePixels, float dpiScale)
{
    // Call the base class helper to re-create resources.
    return createResources(sizePixels.cx, sizePixels.cy);
}

HRESULT __stdcall MainView::getSurface(void** ppSurface)
{
    if (!ppSurface) return E_POINTER;
    *ppSurface = m_sharedSurface.Get();
    if (m_sharedSurface)
        m_sharedSurface->AddRef();
    return S_OK;
}

HRESULT __stdcall MainView::frontBufferAvailableChanged(BOOL nowAvailable)
{
    if (!nowAvailable)
        releaseResources();
    return S_OK;
}