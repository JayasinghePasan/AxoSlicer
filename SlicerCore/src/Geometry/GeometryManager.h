#pragma once
#include "../pch.h"
#include "../../Public/iGeometry.h"
#include "../../Public/iGeometryManager.h"
#include "../../src/Utilities/TranslateBox.h"

class GeometryManager : public ComRoot<iGeometryManager>
{
private:
	std::unordered_map<GUID, iGeometry*, GuidHash, GuidEqual> geometryMap;
	BoundingBox globalBoundingBox;

	TranslateBox translateBox;
	iGeometry* translateGeometry = nullptr;

public:
	// --- iGeometryManager Methods ---
	HRESULT __stdcall AddGeometry(iGeometry* Geometry) override;
	HRESULT __stdcall RemoveGeometry(GUID geometryID) override;
	HRESULT __stdcall RenderGeometries() override;
	HRESULT __stdcall GetGlobalBoundingBox(BoundingBox& box) override;
	HRESULT __stdcall getGeometryCount(int& count) override;
	HRESULT __stdcall SetVisibility(GUID geometryID, BOOL visible) override;
	HRESULT __stdcall setTranslateBox(GUID geomId, bool render) override;
	HRESULT __stdcall PickGeometry(int x, int y, GUID& pickGeomId, RenderState& renderState) override;
	HRESULT __stdcall PickGeomArrow(int x, int y, GUID geomId, BoundingBox globalBB, RenderState& rs, eViewDirection& viewDir) override;
private:
	void RecalcualteBoundingBox();
	

};
