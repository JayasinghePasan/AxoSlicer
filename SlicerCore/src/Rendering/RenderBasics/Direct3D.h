#pragma once
#include "../../pch.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxgi.lib")

// A namespace to hold all global DirectX objects.
namespace Direct3D
{
    extern CComPtr<ID3D11Device>        device11;
    extern CComPtr<ID3D11DeviceContext> context;
    extern CComPtr<IDirect3DDevice9Ex>  device9;
    extern CComAutoCriticalSection      g_contextLock;

    HRESULT createDevices(HWND hWnd);
}
