#pragma once
#include "GeometryManager.h"
#include "../pch.h"

HRESULT createGeometryManager(iGeometryManager** ppGeomManager)
{
    CComPtr<CComObject<GeometryManager>> geometryManager;
    HRESULT hr = createInstance(geometryManager);
    if (FAILED(hr))
        return hr;

    *ppGeomManager = geometryManager.Detach();
    return S_OK;
}


HRESULT __stdcall GeometryManager::AddGeometry(iGeometry* geometry)
{
    GUID geomGuid;
    geometry->GetGuid(geomGuid);

    if ( IsEqualGUID(geomGuid, GUID_NULL))
        return E_FAIL;

    auto it = geometryMap.find(geomGuid);
    if (it != geometryMap.end())
        return E_FAIL;

    geometryMap[geomGuid] = geometry;
    return S_OK;
}

HRESULT __stdcall GeometryManager::RemoveGeometry(iGeometry* geometry)
{
    return E_NOTIMPL;
}

