#pragma once
#include "Direct3D.h"
#include <d3dcompiler.h>

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

    static HRESULT LoadBlobFromFile(LPCWSTR path, CComPtr<ID3DBlob>& blob)
    {
        return D3DReadFileToBlob(path, &blob);
    }

    HRESULT Direct3D::BindShadersFromCSO(const wchar_t* vsCsoPath, const wchar_t* psCsoPath, const D3D11_INPUT_ELEMENT_DESC* layout, 
        UINT layoutCount, ID3D11VertexShader** outVS, ID3D11PixelShader** outPS, ID3D11InputLayout** outIL)
    {
        if (!device11 || !context) 
            return E_FAIL;

        CComPtr<ID3DBlob> vsBlob, psBlob;
        HRESULT hr = LoadBlobFromFile(vsCsoPath, vsBlob);
        if (FAILED(hr)) 
            return hr;

        hr = LoadBlobFromFile(psCsoPath, psBlob);
        if (FAILED(hr)) 
            return hr;

        CComPtr<ID3D11VertexShader> vs;
        CComPtr<ID3D11PixelShader>  ps;

        hr = device11->CreateVertexShader(vsBlob->GetBufferPointer(),
            vsBlob->GetBufferSize(),
            nullptr, &vs);
        if (FAILED(hr)) 
            return hr;

        hr = device11->CreatePixelShader(psBlob->GetBufferPointer(),
            psBlob->GetBufferSize(),
            nullptr, &ps);
        if (FAILED(hr)) 
            return hr;

        CComPtr<ID3D11InputLayout> il;
        hr = device11->CreateInputLayout(
            layout, layoutCount,
            vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(),
            &il);
        if (FAILED(hr)) 
            return hr;

        context->IASetInputLayout(il);
        context->VSSetShader(vs, nullptr, 0);
        context->PSSetShader(ps, nullptr, 0);

        if (outVS) *outVS = vs.Detach();
        if (outPS) *outPS = ps.Detach();
        if (outIL) *outIL = il.Detach();

        return S_OK;
    }

    HRESULT Direct3D::BindComputeShaderFromCSO(const wchar_t* csCsoPath, ID3D11ComputeShader** outCS)
    {
        if (!device11 || !context) 
            return E_FAIL;

        CComPtr<ID3DBlob> csBlob;
        HRESULT hr = LoadBlobFromFile(csCsoPath, csBlob);
        if (FAILED(hr)) 
            return hr;

        CComPtr<ID3D11ComputeShader> cs;
        hr = device11->CreateComputeShader(csBlob->GetBufferPointer(),
            csBlob->GetBufferSize(),
            nullptr, &cs);
        if (FAILED(hr)) 
            return hr;

        context->CSSetShader(cs, nullptr, 0);
        if (outCS) *outCS = cs.Detach();
        return S_OK;
    }
}