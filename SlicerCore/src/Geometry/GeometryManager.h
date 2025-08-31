#pragma once
#include "../../Public/iGeometry.h"
#include "../../Public/iGeometryManager.h"
#include "../pch.h"

class GeometryManager : public ComRoot<iGeometryManager>
{
private:
	std::unordered_map<GUID, iGeometry*, GuidHash, GuidEqual> geometryMap;
public:
	HRESULT __stdcall AddGeometry(iGeometry* Geometry) override;
	HRESULT __stdcall RemoveGeometry(iGeometry* Geometry) override;

};
