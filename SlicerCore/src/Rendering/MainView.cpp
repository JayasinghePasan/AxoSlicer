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
    // Your rendering logic will go here.
    // For now, let's just clear the screen.
    float clearColor[] = { 0.1f, 0.2f, 0.3f, 1.0f };

    // Use the global context pointer from Direct3D.h
    if (Direct3D::context && m_renderTargetView)
    {
        Direct3D::context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
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
    // We'll implement this later.
    return S_OK;
}