#pragma once

#include "pch.h"

// Link the necessary libraries so you don't have to configure them in the project properties.
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxgi.lib")

// A namespace to hold all global DirectX objects.
namespace Direct3D
{
    // --- Global Device Pointers ---
    // 'extern' tells the compiler that these variables exist, but they are defined
    // in another file (Direct3D.cpp). This allows multiple files to share them.
    extern CComPtr<ID3D11Device>        device;
    extern CComPtr<ID3D11DeviceContext> context;
    extern CComPtr<IDirect3DDevice9Ex>  device9;

    // A critical section to lock the D3D11 context for thread safety.
    extern CComAutoCriticalSection      g_contextLock;

    // --- Global Factory Function ---
    // This is the function that creates the devices defined above.
    HRESULT createDevice(HWND hWnd);
}
