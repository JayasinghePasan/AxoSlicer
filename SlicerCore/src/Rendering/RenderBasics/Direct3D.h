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
    
    HRESULT BindShadersFromCSO(const wchar_t* vsCsoPath, const wchar_t* psCsoPath,
        const D3D11_INPUT_ELEMENT_DESC* layout, UINT layoutCount,
        ID3D11VertexShader** outVS = nullptr,
        ID3D11PixelShader** outPS = nullptr,
        ID3D11InputLayout** outIL = nullptr);

    // (Optional) bind a compute shader from .cso
    HRESULT BindComputeShaderFromCSO( const wchar_t* csCsoPath, ID3D11ComputeShader** outCS = nullptr);
}
