#pragma once
#include "Geometry.h"
#include "../pch.h"

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

    return S_OK;
}

HRESULT __stdcall Geometry::Render()
{
    throw E_NOTIMPL;
}

HRESULT __stdcall Geometry::GetGuid(GUID& guid)
{
    guid = geometryID;
    return S_OK;
}
