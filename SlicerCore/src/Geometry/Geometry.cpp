#pragma once
#include "Geometry.h"
#include "../pch.h"
#include "../Rendering/RenderBasics/Direct3D.h"

using namespace Direct3D;

HRESULT createGeometry(const void* buffer, size_t length, iGeometry** ppGeometry)
{
    if (!buffer )
        return E_POINTER;

    CComPtr<CComObject<Geometry>> geometry;
    HRESULT hr = createInstance(geometry);
    if (FAILED(hr))
        return hr;

    hr = geometry->LoadFromBuffer(buffer, length);
    if (FAILED(hr))
        return hr;

    *ppGeometry = geometry.Detach();
    return S_OK;
}


HRESULT Geometry::LoadFromBuffer(const void* buffer, size_t length)
{
    if (!buffer || length < 84) 
        return E_FAIL;

    // assign the unique geometry id
    HRESULT hr = CoCreateGuid(&geometryID);
    if (FAILED(hr))
        return E_FAIL;

    const char* ptr = static_cast<const char*>(buffer);

    char header[81] = {};
    memcpy(header, ptr, 80);
    ptr += 80;

    uint32_t triCount = *reinterpret_cast<const uint32_t*>(ptr);
    ptr += 4;

    if (length < 84ull + 50ull * triCount)
        return E_FAIL; 

    triangles.reserve(triCount);
    for (uint32_t i = 0; i < triCount; ++i)
    {
        Triangle t;
        memcpy(&t, ptr, sizeof(Triangle));
        triangles.push_back(t);
        ptr += sizeof(Triangle);
    }

    UploadToGPUBuffers();
    return S_OK;
}

HRESULT __stdcall Geometry::Render()
{
    if (!context || !vertexBuffer)
        return E_FAIL;

    D3D11_INPUT_ELEMENT_DESC layout[] = 
    {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
      D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    HRESULT hr = Direct3D::BindShadersFromFiles( L"\\..\\Rendering\\Shaders\\SimpleVS.hlsl", L"..\\Rendering\\Shaders\\SimplePS.hlsl", layout, _countof(layout)); 
    if (FAILED(hr))
        return S_FALSE;
    UINT stride = sizeof(float) * 3;
    UINT offset = 0;
    ID3D11Buffer* vb = vertexBuffer.Get();

    context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(static_cast<UINT>(triangles.size() * 3), 0);
}

HRESULT __stdcall Geometry::GetGuid(GUID& guid)
{
    guid = geometryID;
    return S_OK;
}

void Geometry::UploadToGPUBuffers()
{
    // if already uploaded
    if (vertexBuffer)
        return;

    if (triangles.empty())
        return;
    
    std::vector<Vertex> vertices;
    vertices.reserve(triangles.size() * 3);

    for (const Triangle& tri : triangles)
    {
        vertices.push_back(Vertex(tri.v1[0], tri.v1[1], tri.v1[2]));
        vertices.push_back(Vertex(tri.v2[0], tri.v2[1], tri.v2[2]));
        vertices.push_back(Vertex(tri.v3[0], tri.v3[1], tri.v3[2]));
    }

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex));
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices.data();

    HRESULT hr = device11->CreateBuffer(&bd, &initData, &vertexBuffer);
    if (FAILED(hr))
    {
        vertexBuffer.Reset();
        return;
    }

    triangles.clear();
    return;
}
