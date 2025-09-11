#pragma once
#include "iGeometry.h"
#include <Unknwn.h>

// Manages and keeps the Geometrys in the backend
__interface __declspec(uuid("b8c243af-ad61-41e5-a8a0-89579b988b1d")) iGeometryManager : public IUnknown
{
	HRESULT __stdcall AddGeometry(iGeometry* Geometry);
	HRESULT __stdcall RemoveGeometry(GUID geometryID);
	HRESULT __stdcall RenderGeometries();
	HRESULT __stdcall GetGlobalBoundingBox(BoundingBox& box);
	HRESULT __stdcall getGeometryCount(int& count);
	HRESULT __stdcall SetVisibility(GUID geometryID, BOOL visible);
	HRESULT __stdcall setTranslateBox(GUID geomId, bool render);
	HRESULT __stdcall PickGeometry(int x, int y, GUID& pickGeomId, RenderState& renderState);
	HRESULT __stdcall PickGeomArrow(int x, int y, GUID geomId, BoundingBox globalBB, RenderState& rs, eViewDirection& viewDir);
};