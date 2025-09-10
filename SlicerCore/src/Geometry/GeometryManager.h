#pragma once
#include "../../Public/iGeometry.h"
#include "../../Public/iGeometryManager.h"
#include "../pch.h"

class GeometryManager : public ComRoot<iGeometryManager>
{
private:
	std::unordered_map<GUID, iGeometry*, GuidHash, GuidEqual> geometryMap;
	BoundingBox globalBoundingBox;
public:
	// --- iGeometryManager Methods ---
	HRESULT __stdcall AddGeometry(iGeometry* Geometry) override;
	HRESULT __stdcall RemoveGeometry(GUID geometryID) override;
	HRESULT __stdcall RenderGeometries() override;
	HRESULT __stdcall GetGlobalBoundingBox(BoundingBox& box) override;
	HRESULT __stdcall getGeometryCount(int& count) override;
	HRESULT __stdcall SetVisibility(GUID geometryID, BOOL visible) override;
	HRESULT __stdcall PickGeometry(int x, int y, GUID& pickGeomId, RenderState& renderState) override;

private:
	void RecalcualteBoundingBox();
	

};
