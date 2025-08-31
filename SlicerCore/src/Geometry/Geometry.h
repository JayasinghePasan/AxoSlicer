#pragma once
#include "../../Public/iGeometry.h"
#include "../pch.h"

class Geometry : public ComRoot<iGeometry>
{
public:
	// iGeometry
	HRESULT __stdcall GetGuid(GUID& guid) override;
	HRESULT __stdcall Render() override;

	HRESULT LoadFromBuffer(const void* buffer, size_t length);

private:
	GUID geometryID;					// set when loading from the buffer
	std::vector<Triangle> triangles;
};
