#pragma once
#include "../../pch.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

// A namespace to hold all global DirectX objects.
namespace Direct3D
{
    extern CComPtr<ID3D11Device>        device11;
    extern CComPtr<ID3D11DeviceContext> context;
    extern CComPtr<IDirect3DDevice9Ex>  device9;
    extern CComAutoCriticalSection      g_contextLock;

    extern CComPtr<ID3D11VertexShader>  simpleVertexShader;
    extern CComPtr<ID3D11PixelShader>   simplePixelShader;
    extern CComPtr<ID3D11InputLayout>   simpleInputLayout;

    HRESULT createDevices(HWND hWnd);
    HRESULT BindShadersFromFiles(
        const wchar_t* vsPath,
        const wchar_t* psPath,
        const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutCount,
        const char* vsEntry = "VSMain",
        const char* psEntry = "PSMain",
        const char* vsModel = "vs_5_0",
        const char* psModel = "ps_5_0",
        ID3D11VertexShader** outVS = nullptr,
        ID3D11PixelShader** outPS = nullptr,
        ID3D11InputLayout** outIL = nullptr);
}
