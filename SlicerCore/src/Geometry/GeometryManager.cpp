#pragma once
#include "Geometry.h"
#include "GeometryManager.h"
#include "../Rendering/MousePickers/GeometryPicker.h"
#include "../Rendering/RenderBasics/Direct3D.h"
#include "../pch.h"

HRESULT createGeometryManager(iGeometryManager** ppGeomManager)
{
    if (!ppGeomManager)
       return E_POINTER;

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

    RecalcualteBoundingBox();

    return S_OK;
}

HRESULT __stdcall GeometryManager::RemoveGeometry(GUID geometryID)
{
    geometryMap.erase(geometryID);
    RecalcualteBoundingBox();
    return S_OK;
}

HRESULT __stdcall GeometryManager::RenderGeometries()
{
    // set common IA and shaders for geometries
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    HRESULT hr = Direct3D::BindShadersFromCSO(L"SimpleVS.cso", L"SimplePS.cso", layout, _countof(layout));

    if (FAILED(hr))
        return hr;

    for (auto it : geometryMap)
    {
        if (it.second)
            it.second->Render();
    }
    return S_OK;
}

HRESULT __stdcall GeometryManager::GetGlobalBoundingBox(BoundingBox& box)
{
    box = globalBoundingBox;
    return S_OK;
}

HRESULT __stdcall  GeometryManager::getGeometryCount(int& count)
{
    count = geometryMap.size();
    return S_OK;
}

void GeometryManager::RecalcualteBoundingBox()
{
    globalBoundingBox.minX = globalBoundingBox.minY = globalBoundingBox.minZ =  std::numeric_limits<float>::infinity();
    globalBoundingBox.maxX = globalBoundingBox.maxY = globalBoundingBox.maxZ = -std::numeric_limits<float>::infinity();

    for (auto geom : geometryMap)
    {
        BoundingBox geomBox;
        geom.second->GetBoundingBox(geomBox);
        globalBoundingBox.expandToInclude(geomBox);
    }
}

HRESULT __stdcall GeometryManager::SetVisibility(GUID geometryID, BOOL visible)
{
    auto it = geometryMap.find(geometryID);
    if (it == geometryMap.end())
        return E_FAIL;

    Geometry* geomRaw = dynamic_cast<Geometry*>(it->second);
    if (!geomRaw)
        return E_FAIL;

    geomRaw->SetVisibility(visible);
    RecalcualteBoundingBox();
    return S_OK;
}

HRESULT __stdcall GeometryManager::PickGeometry(int x, int y, GUID& pickGeomId, RenderState& renderState)
{
    GeometryPicker gp(geometryMap, renderState);
    gp.Pick(x, y);
    gp.ReadGeometry(pickGeomId);
    return S_OK;
}

