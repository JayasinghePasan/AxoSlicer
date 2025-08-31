#pragma once
#include "Direct3D.h"

namespace Direct3D
{
    // Define the global pointers that were declared in the header.
    CComPtr<ID3D11Device> device11;
    CComPtr<ID3D11DeviceContext> context;
    CComPtr<IDirect3DDevice9Ex> device9;
    CComAutoCriticalSection g_contextLock;

    
    HRESULT createDevices(HWND hWnd)
    {
        if (device11)
            return S_OK;

        // --- Create D3D11 Device ---
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
        creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT hr = D3D11CreateDevice(
            nullptr,                    // Default adapter
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,                    // No software module
            creationFlags,
            nullptr,                    // Default feature levels
            0,
            D3D11_SDK_VERSION,
            &device11,
            nullptr,
            &context);

        if (FAILED(hr))
            return hr;

        // --- Create D3D9 Device for WPF Interop ---
        CComPtr<IDirect3D9Ex> d3d9Context;
        hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9Context);
        if (FAILED(hr))
            return hr;

        D3DPRESENT_PARAMETERS d3d9Params = {};
        d3d9Params.Windowed = TRUE;
        d3d9Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3d9Params.hDeviceWindow = hWnd;
        d3d9Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

        hr = d3d9Context->CreateDeviceEx(
            D3DADAPTER_DEFAULT,
            D3DDEVTYPE_HAL,
            hWnd,
            D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED,
            &d3d9Params,
            nullptr,
            &device9);

        return hr;
    }
}